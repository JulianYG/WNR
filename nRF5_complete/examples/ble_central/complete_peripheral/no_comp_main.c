/*
 * Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is confidential property of Nordic Semiconductor. The use,
 * copying, transfer or disclosure of such information is prohibited except by express written
 * agreement with Nordic Semiconductor.
 *
 */

#include <stdint.h>
#include <string.h>
#include "nrf.h"
#include "nordic_common.h"
#include "ble.h"
#include "ble_gap.h"
#include "ble_hci.h"
#include "app_error.h"
#include "app_timer.h"
//#include "app_button.h"
#include "ble_advdata.h"
#include "boards.h"
#include "nrf_gpio.h"
#include "nrf_sdm.h"
#include "softdevice_handler.h"
#include "pstorage.h"
#include "device_manager.h"
//#include "bsp.h"
//#include "bsp_btn_ble.h"
#include "ble_advertising.h"

#include "app_uart.h"

#include "nrf_delay.h"
#include "app_util_platform.h"
#include "nrf_drv_spi.h"
#include "bsp.h"

#include "heatshrink_encoder.h"
#include "heatshrink_decoder.h"
#include "heatshrink_config.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "nrf_drv_clock.h"
#include "nrf_soc.h"

#include "nrf_drv_gpiote.h"

#define IS_SRVC_CHANGED_CHARACT_PRESENT    0                                          /**< Include or not the service_changed characteristic. If not enabled, the server's database cannot be changed for the lifetime of the device. */

#define CENTRAL_LINK_COUNT                 0                                          /**<number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT              1                                          /**<number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/

#define UART_TX_BUF_SIZE        256                             /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE        256                             /**< UART RX buffer size. */

#define SEND_NOTIFICATION_BUTTON_ID        0                                          /**< Id for button used for sending notification button. */

#define DEVICE_NAME                        "Multilink_SX"                                /**< Name of device. Will be included in the advertising data. */

#define APP_TIMER_PRESCALER                0                                          /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE            4                                          /**< Size of timer operation queues. */

#define APP_ADV_INTERVAL                   MSEC_TO_UNITS(50, UNIT_0_625_MS)           /**< The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS         180                                        /**< The advertising timeout (in units of seconds). */

#define MIN_CONN_INTERVAL                  MSEC_TO_UNITS(7.5, UNIT_1_25_MS)           /**< Minimum acceptable connection interval (0.5 seconds). */
#define MAX_CONN_INTERVAL                  MSEC_TO_UNITS(7.5, UNIT_1_25_MS)          /**< Maximum acceptable connection interval (1 second). */
#define SLAVE_LATENCY                      0                                          /**< Slave latency. */
#define CONN_SUP_TIMEOUT                   MSEC_TO_UNITS(4000, UNIT_10_MS)            /**< Connection supervisory timeout (4 seconds). */

#define MULTILINK_PERIPHERAL_BASE_UUID     {{0xB3, 0x58, 0x55, 0x40, 0x50, 0x60, 0x11, \
                                           0xe3, 0x8f, 0x96, 0x08, 0x00, 0x00, 0x00,   \
                                           0x9a, 0x66}}                               /**< 128bit UUID base used for example. */
#define MULTILINK_PERIPHERAL_SERVICE_UUID  0x9001                                     /**< Serrvice UUID over the 128-bit base used for the example. */
#define MULTILINK_PERIPHERAL_CHAR_UUID     0x900A                                     /**< Characteristic UUID over the 128-bit base used for the example. */

#define SEC_PARAM_TIMEOUT                  30                                         /**< Timeout for Pairing Request or Security Request (in seconds). */
#define SEC_PARAM_BOND                     1                                          /**< Perform bonding. */
#define SEC_PARAM_MITM                     0                                          /**< Man In The Middle protection not required. */
#define SEC_PARAM_IO_CAPABILITIES          BLE_GAP_IO_CAPS_NONE                       /**< No I/O capabilities. */
#define SEC_PARAM_OOB                      0                                          /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE             7                                          /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE             16

#define MAX_DATA_LENGTH 20 /**<Maximum number of bytes in one transmission>*/

#define BLE_INIT_PERIOD APP_TIMER_TICKS(10, APP_TIMER_PRESCALER) // Initializing ble connection in the beginning attempt rate: 10 ms
																					 
static uint16_t                  m_conn_handle = BLE_CONN_HANDLE_INVALID;             /**< Connection handle. */
static ble_gatts_char_handles_t  m_char_handles;                                      /**< GATT characteristic definition handles. */
static uint8_t                   m_base_uuid_type;                                    /**< UUID type. */
static dm_application_instance_t m_app_handle;                                        /**< Application instance allocated by device manager to the application. */
static bool clear_to_send = false;
static bool connected = false;                           /**<State of BLE connection>*/
static bool rtos_running = false;                           /**<Used to determine if rtos has been started or not (whether to resume or start scheduler)>*/
static bool ble_initialized = false;      /**<In the beginning, transmission will error.  Once we sort this out, this flag will be set true.>*/					
static bool first_tx_successful = false;  /**<Used to determine when to stop testing connection when first connected.>**/
app_timer_id_t ble_init_timer_id;

/********************************** DATA ACQUISITION DEFINITIONS ************************************************/

//#define APP_TIMER_MAX_TIMERS     BSP_APP_TIMERS_NUMBER  ///< Maximum number of simultaneously created timers.
#define SAMPLE_DELAY APP_TIMER_TICKS(1, APP_TIMER_PRESCALER) // Sample rate in MS
#define NUM_CHANNELS 2                                         // Number of channels to sample from

#define DELAY_MS                 10 /*1000*/                   ///< Timer Delay in milli-seconds.

//ADC setup
int ADC_bias = 32; // <= 120 kS/s
int MUX_bias = 40; // <= 120 kS/s
int ADC_twoscomp = 0x40; // no weakMISO, twoscomp, no absmode, no DSP
//high cutoff
int RH1_DAC1 = 30;  //reg 8
int RH1_DAC2 = 5;   // reg 9
int RH2_DAC1 = 43;  //reg  10
int RH2_DAC2 = 6;   //reg 11
//low cutoff
int RL_DAC1 = 16;   //reg  12
int RL_DAC2 = 60;   //reg 13
int RL_DAC3 = 1;    //reg 13 - bit 6
/** @def  TX_RX_MSG_LENGTH
 * number of bytes to transmit and receive. This amount of bytes will also be tested to see that
 * the received bytes from slave are the same as the transmitted bytes from the master */
#define TX_MSG_LENGTH         2  // 16-bits command words
#define RX_MSG_LENGTH 		  2  // 16-bits echo

typedef enum
{
    TEST_STATE_SPI0_LSB,    ///< Test SPI0, bits order LSB
    TEST_STATE_SPI0_MSB,    ///< Test SPI0, bits order MSB
    END_OF_TEST_SEQUENCE
} spi_master_ex_state_t;

typedef enum{
	CALIBRATE,
	CLEAR,
	WRITE,
	READ,
	CONVERT,
	DUMMY
} intan_function;

static intan_function m_intan_function = DUMMY; //initialize state for DUMMY


static uint8_t m_tx_data_spi[TX_MSG_LENGTH]; ///< SPI master TX buffer.
static uint8_t m_rx_data_spi[RX_MSG_LENGTH]; ///< SPI master RX buffer.

static volatile bool m_transfer_completed = true;
static spi_master_ex_state_t m_spi_master_ex_state = (spi_master_ex_state_t)0;

static const nrf_drv_spi_t m_spi_master_0 = NRF_DRV_SPI_INSTANCE(0);
static int intan_convert_channel = 0;          // Channel on Intan currently converting.
//static uint32_t counter = 0;
/********************************** COMPRESSION DEFINITIONS ************************************************/

#define NUM_DATA_BUFFERS 2                                                     /**<Number of data buffers to use> */
#define DATA_BUF_SIZE 	(2048U)                                                /**<buffer size for each data buffer.> */
#define TRANSMISSION_BUF_SIZE 	(DATA_BUF_SIZE + (DATA_BUF_SIZE/2) + 4)          /**<buffer size for the transmission buffer.> */
#define COMP_DELAY 1                                        /** <Delay compression by 1 ms each time> */
#define SEND_DELIMETER 0xFFFFFFFF    /**<Write this to the corresponding tx_buffers_pending index to denote that the delimeter packet will be sent>*/
#define EMPTY_TX_BUFF_VAL -1

static uint8_t *transmission_buffers[NUM_DATA_BUFFERS];                        /**<Buffer for storing and sending the compressed data>*/
static uint8_t *data_buffers[NUM_DATA_BUFFERS];                                /**<Buffers for storing data>*/
static bool comp_buffers_ready[NUM_DATA_BUFFERS];                              /**<At each index, stores whether or not the compression buffer is full of raw data>*/
static int32_t tx_buffers_pending[NUM_DATA_BUFFERS];                          /**<At each index, stores the amount of data needed to be transmitted>*/
static bool tx_in_progress = false;                                            /**<True if currently transmitting a buffer, false otherwise>*/
static uint8_t active_buffer = 0;                                              /**<Buffer index that is currently storing incoming data>*/
static uint8_t comp_index = 0;                                                 /**<Buffer index that will be filled and compressed next>*/
static uint8_t tx_index = 0;                                                   /**<Tx Buffer index that is either currently transmitting or next to be transmitted>*/
static uint32_t curr_tx_size = 0;                                              /**<The total buffer size (after compression) tx buffer current transmitting>*/
static uint16_t ab_capacity = DATA_BUF_SIZE;                                   /**<How much more space is left in the active buffer>*/

//static bool compression_in_progress = false;                                 /**<Flag for whether or not is in progress>*/

static heatshrink_encoder hse;

/**<Packet filled with zeroes to be appended to non-full length packets>*/
static uint8_t zeroes_packet[MAX_DATA_LENGTH] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	
/**<Delimiter packet to be sent in between buffers. Set to "TKENDTKENDTKENDTKEND>*/
static uint8_t buf_delimiter[MAX_DATA_LENGTH] = {84, 75, 69, 78, 68, 84, 75, 69, 78, 68, 84, 75, 69, 78, 68, 84, 75, 69, 78, 68};
static char t_length[4];
static uint8_t data_to_send = 65;

/********************************** TRANSMISSION PROTOTYPE ************************************************/
static void data_send(uint8_t buf, uint32_t size);

/********************************** DATA ACQUISITION PROTOTYPE ************************************************/

static void intan_dummy(uint8_t * const p_tx_buf,
                     uint8_t * const p_rx_buf);
static void intan_calibrate(uint8_t * const p_tx_buf,
                     uint8_t * const p_rx_buf);
static void intan_clear(uint8_t * const p_tx_buf,
                     uint8_t * const p_rx_buf);
static void intan_write(uint8_t * const p_tx_buf,
                     uint8_t * const p_rx_buf,
                     int R, //register
                     int D); //data
static void intan_read(uint8_t * const p_tx_buf,
                     uint8_t * const p_rx_buf,
                     int R); //register
static void intan_convert(uint8_t * const p_tx_buf,
                     uint8_t * const p_rx_buf,
                     int C); //channel
void app_error_handler_main(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name);
void uart_error_handle(app_uart_evt_t * p_event);
void spi_master_0_event_handler(nrf_drv_spi_evt_type_t* event);
//void *spi_master_0_event_handler_implementation(nrf_drv_spi_evt_type_t* event);
static void spi_master_init(nrf_drv_spi_t const * p_instance, bool lsb);
static void spi_send_recv(nrf_drv_spi_t const * p_instance,
                          uint8_t * p_tx_data,
                          uint8_t * p_rx_data,
                          uint16_t  tx_len,
                          uint16_t  rx_len);

//APP_TIMER_DEF(spi_timer);        // For data acquisition

static void switch_state(void);
void bsp_configuration(void);
void intan_setup(void);
void timers_init(void);
void data_collection_timers_start(void);
void sample_data(void);


/********************************** COMPRESSION PROTOTYPE ************************************************/
void buffers_init(void);
void compress(uint32_t buf, uint32_t size);



/********************************** DATA ACQUISITION FUNCTIONS ************************************************/

/** The following driver functions do the following:
* populate tx_buffer
* clear rx_buffer
* set relevant m_intan_function enum 
*/
static void intan_dummy(uint8_t * const p_tx_buf,
                     uint8_t * const p_rx_buf)
{
	p_tx_buf[0] = 0x00; //set 0x0000 to transmit
	p_tx_buf[1] = 0x00;
	p_rx_buf[0] = 0x00; // clear rx buffer
	p_rx_buf[1] = 0x00;	

	m_intan_function = DUMMY; // set function to be dummy

}

static void intan_calibrate(uint8_t * const p_tx_buf,
                     uint8_t * const p_rx_buf)
{
	p_tx_buf[0] = 0x00; //set 0x5500 to transmit
	p_tx_buf[1] = 0x00;
	p_rx_buf[0] = 0x00; // clear rx buffer
	p_rx_buf[1] = 0x00;	
	
	m_intan_function = CALIBRATE; // set function to be dummy

}
static void intan_clear(uint8_t * const p_tx_buf,
                     uint8_t * const p_rx_buf)
{
	p_tx_buf[0] = 0x6A; //set 0x6A00 to transmit
	p_tx_buf[1] = 0x00;
	p_rx_buf[0] = 0x00; // clear rx buffer
	p_rx_buf[1] = 0x00;	
	
	m_intan_function = CLEAR; // set function to be dummy

}
static void intan_write(uint8_t * const p_tx_buf,
                     uint8_t * const p_rx_buf,
                     int R, //register
                     int D) //data
{
	int message = 0x8000 | (R<<8 | D);
	p_tx_buf[0] = message >> 8; //set 0x0000 to transmit
	p_tx_buf[1] = message & 0xFF;
	p_rx_buf[0] = 0x00; // clear rx buffer
	p_rx_buf[1] = 0x00;	
	
	m_intan_function = WRITE; // set function to be dummy

}
static void intan_read(uint8_t * const p_tx_buf,
                     uint8_t * const p_rx_buf,
                     int R) //register
{
	int message = 0xC000 | (R<<8);
	p_tx_buf[0] = message >> 8; //set 0x0000 to transmit
	p_tx_buf[1] = message & 0xFF;
	p_rx_buf[0] = 0x00; // clear rx buffer
	p_rx_buf[1] = 0x00;	
	
	m_intan_function = READ; // set function to be dummy
}

static void intan_convert(uint8_t * const p_tx_buf,
                     uint8_t * const p_rx_buf,
                     int C) //channel
{
	int message = (C<<8);
	p_tx_buf[0] = message >> 8; //set 0x0000 to transmit
	p_tx_buf[1] = message & 0xFF;
	p_rx_buf[0] = 0x00; // clear rx buffer
	p_rx_buf[1] = 0x00;	
	
	m_intan_function = CONVERT; // set function to be dummy
}

void uart_error_handle(app_uart_evt_t * p_event)
{
    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_communication);
    }
    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
}

/**@brief Function for error handling, which is called when an error has occurred. 
 *
 * @param[in] error_code  Error code supplied to the handler.
 * @param[in] line_num    Line number where the handler is called.
 * @param[in] p_file_name Pointer to the file name. 
 */
void app_error_handler_main(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
    //UNUSED_VARIABLE(bsp_indication_set(BSP_INDICATE_FATAL_ERROR));

    for (;;)
    {
        // No implementation needed.
    }
}


/**@brief Handler for SPI0 master events.
 *
 * @param[in] event SPI master event.
 */
void spi_master_0_event_handler(nrf_drv_spi_evt_type_t* event)
{
    uint32_t err_code = NRF_SUCCESS;
	  //uint32_t c_buff;   // Temporarily store the buffer index of the buffer ready to be compressed

    switch (*event)
    {
			  //while(app_uart_put(73) != NRF_SUCCESS);
			  // Transfer complete; store in buffer.
        case NRF_DRV_SPI_EVENT_DONE:
					
            nrf_drv_spi_uninit(&m_spi_master_0);
				
				    // Transfer done after this point
						m_transfer_completed = true;
				
				    //printf("Data received\r\n");
				    //while(app_uart_put(69) != NRF_SUCCESS);
				    //printf("%d\r\n", ++counter);

            //err_code = bsp_indication_set(BSP_INDICATE_RCV_OK);
            //APP_ERROR_CHECK(err_code);*/
				
				    // All buffers are in use; The just acquired data will be lost.
				    if(comp_buffers_ready[active_buffer])
						{
							while(app_uart_put(70) != NRF_SUCCESS);
							//while(app_uart_put(70) != NRF_SUCCESS);
							//printf("All buffers in use. Data lost\r\n");
						}
						
						// Still have space to store data.
						else
						{
							// Current buffer still has space
							if(ab_capacity > 0)
							{
								//*(data_buffers[active_buffer] + DATA_BUF_SIZE - ab_capacity) = m_rx_data_spi[0];
								//*(data_buffers[active_buffer] + DATA_BUF_SIZE - ab_capacity) = 1;
								//data_buffers[active_buffer][DATA_BUF_SIZE - ab_capacity] = data_to_send;
								
								// Buffer is filled after this storage.  Set the next buffer as active.  Call compression, if compression is not in progress.
								if(--ab_capacity == 0)
								{
									comp_buffers_ready[active_buffer] = true;
									ab_capacity = DATA_BUF_SIZE;
									//data_to_send++;
									if(active_buffer == NUM_DATA_BUFFERS - 1)
									{
										active_buffer = 0;
									}
									
									else
									{
										active_buffer++;
									}
									
									// Compress
									//if(!compression_in_progress)
									//{
										//printf("Compressing\r\n");
										//while(app_uart_put(73) != NRF_SUCCESS);
										//compression_in_progress = true;
										//err_code = app_timer_start(comp_timer, 5, &c_buff);
										//while(app_uart_put(74) != NRF_SUCCESS);
										//printf("%d\r\n", err_code);
										//while(1);
										//APP_ERROR_CHECK(err_code);
										//while(app_uart_put(75) != NRF_SUCCESS);
										//compress(c_buff, DATA_BUF_SIZE);
										//printf("Compress is done\r\n");
										//while(app_uart_put(71) != NRF_SUCCESS);
									//}
								}
							}
						}
						
						// If not all channels have been sampled, continue sampling from the other channels.
						if(intan_convert_channel == NUM_CHANNELS)
						{
							intan_convert_channel = 0;
						}
						else
						{
							sample_data();
						}
            break;

        default:
            // No implementation needed.
            break;
    }
}

/**@brief Handler for SPI0 company Intan read events.
 *
 * @param[in] event SPI master event.
 */
void spi_intan_id_read_handler(nrf_drv_spi_evt_type_t* event)
{
    uint32_t err_code = NRF_SUCCESS;
	  //uint32_t c_buff;   // Temporarily store the buffer index of the buffer ready to be compressed

    switch (*event)
    {
			  //while(app_uart_put(73) != NRF_SUCCESS);
			  // Transfer complete; store in buffer.
        case NRF_DRV_SPI_EVENT_DONE:
					
            nrf_drv_spi_uninit(&m_spi_master_0);
				
						//printf("%c\r\n",m_rx_data_spi[1]);
				    // Transfer done after this point
						m_transfer_completed = true;
				
				    //printf("Data received\r\n");
				    //while(app_uart_put(69) != NRF_SUCCESS);
				    //printf("%d\r\n", ++counter);

            //err_code = bsp_indication_set(BSP_INDICATE_RCV_OK);
            //APP_ERROR_CHECK(err_code);*/
				
				    // All buffers are in use; The just acquired data will be lost.
				    /*if(buffers_ready[active_buffer])
						{
							while(app_uart_put(70) != NRF_SUCCESS);
							//while(app_uart_put(70) != NRF_SUCCESS);
							//printf("All buffers in use. Data lost\r\n");
						}
						
						// Still have space to store data.
						else
						{
							// Current buffer still has space
							if(ab_capacity > 0)
							{
								*(data_buffers[active_buffer] + DATA_BUF_SIZE - ab_capacity) = m_rx_data_spi[0];
								
								// Buffer is filled after this storage.  Set the next buffer as active.  Call compression, if compression is not in progress.
								if(--ab_capacity == 0)
								{
									buffers_ready[active_buffer] = true;
									ab_capacity = DATA_BUF_SIZE;
									if(active_buffer == NUM_DATA_BUFFERS - 1)
									{
										active_buffer = 0;
									}
									
									else
									{
										active_buffer++;
									}
									
									// Compress
									//if(!compression_in_progress)
									//{
										//printf("Compressing\r\n");
										//while(app_uart_put(73) != NRF_SUCCESS);
										//compression_in_progress = true;
										//err_code = app_timer_start(comp_timer, 5, &c_buff);
										//while(app_uart_put(74) != NRF_SUCCESS);
										//printf("%d\r\n", err_code);
										//while(1);
										//APP_ERROR_CHECK(err_code);
										//while(app_uart_put(75) != NRF_SUCCESS);
										//compress(c_buff, DATA_BUF_SIZE);
										//printf("Compress is done\r\n");
										//while(app_uart_put(71) != NRF_SUCCESS);
									//}
								}
							}
						}
						
						// If not all channels have been sampled, continue sampling from the other channels.
						if(intan_convert_channel == NUM_CHANNELS)
						{
							intan_convert_channel = 0;
						}
						else
						{
							sample_data();
						}*/
            break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for initializing a SPI master driver.
 *
 * @param[in] p_instance    Pointer to SPI master driver instance.
 * @param[in] lsb           Bits order LSB if true, MSB if false.
 */
static void spi_master_init(nrf_drv_spi_t const * p_instance, bool lsb)
{
    uint32_t err_code = NRF_SUCCESS;

    nrf_drv_spi_config_t config =
    {
        .irq_priority = APP_IRQ_PRIORITY_LOW,
        .orc          = 0xCC,
        .frequency    = NRF_DRV_SPI_FREQ_8M,   // originally NRF_DRV_SPI_FREQ_1M
        .mode         = NRF_DRV_SPI_MODE_0,
        .bit_order    = (lsb ? //if lsb then LSB_FIRRST, else MSB_FIRST
            NRF_DRV_SPI_BIT_ORDER_LSB_FIRST : NRF_DRV_SPI_BIT_ORDER_MSB_FIRST),
    };

    if (p_instance == &m_spi_master_0)
    {
        config.sck_pin  = SPIM0_SCK_PIN;
        config.mosi_pin = SPIM0_MOSI_PIN;
        config.miso_pin = SPIM0_MISO_PIN; // SS not initialized
				config.ss_pin 	= SPIM0_SS_PIN;
			  err_code = nrf_drv_spi_init(p_instance, &config,
				//spi_intan_id_read_handler);           // EDIT: INTAN ID spi handler
				spi_master_0_event_handler);    // EDIT: data collection spi handler
    }

    APP_ERROR_CHECK(err_code);
}


/**@brief Function for sending and receiving data.
 *
 * @param[in]   p_instance   Pointer to SPI master driver instance.
 * @param[in]   p_tx_data    A pointer to a buffer TX.
 * @param[out]  p_rx_data    A pointer to a buffer RX.
 * @param[in]   len          A length of the data buffers.
 */
static void spi_send_recv(nrf_drv_spi_t const * p_instance,
                          uint8_t * p_tx_data,
                          uint8_t * p_rx_data,
                          uint16_t  tx_len,
                          uint16_t  rx_len)
{
    uint32_t err_code = nrf_drv_spi_transfer(p_instance,
        p_tx_data, tx_len, p_rx_data, rx_len);
    //APP_ERROR_CHECK(err_code);
}


/**@brief Function for executing and switching state.
 *
 */
static void switch_state(void)
{
    nrf_drv_spi_t const * p_instance;

    p_instance = &m_spi_master_0;
    // check output of each function 

    spi_master_init(p_instance, false); // false - MSB first
    spi_send_recv(p_instance, m_tx_data_spi, m_rx_data_spi , TX_MSG_LENGTH, RX_MSG_LENGTH);
}

/**@brief Function for initializing bsp module.
 */
void bsp_configuration()
{
    uint32_t err_code = NRF_SUCCESS;


    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;	
    NRF_CLOCK->LFCLKSRC            = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_LFCLKSTART    = 1;

    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0)
    {
        // Do nothing.
    }
		
	  // Set the external high frequency clock source to 32 MHz
    /*NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
		NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART    = 1;

    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0)
    {
        // Do nothing.
    }*/

    /*APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, NULL);

    err_code = bsp_init(BSP_INIT_LED, APP_TIMER_TICKS(100, APP_TIMER_PRESCALER), NULL);
    APP_ERROR_CHECK(err_code);*/
}
void intan_setup(void){
	int setup_reg[13] = {1,2,4,8,9,10,11,12,13,14,15,16,17};
	int setup_data[13] = {ADC_bias & 0x3F, MUX_bias, ADC_twoscomp, RH1_DAC1,RH1_DAC2,RH2_DAC1,RH2_DAC2,RL_DAC1,((RL_DAC3 << 6) + RL_DAC2),0xFF,0xFF,0xFF,0xFF};

	for (int i =0; i<13; i++){
		while (!m_transfer_completed){};

    	m_transfer_completed = false;
    	intan_write(m_tx_data_spi, m_rx_data_spi,setup_reg[i],setup_data[i]);
    	switch_state();
    	nrf_delay_ms(DELAY_MS);
    	
    }
    for (int j =0; j<2; j++){
		while (!m_transfer_completed){};
    	        
    	m_transfer_completed = false;
    	intan_dummy(m_tx_data_spi, m_rx_data_spi);
    	switch_state();
    	nrf_delay_ms(DELAY_MS);
  
    }
}

/********************************** COMPRESSION FUNCTIONS ************************************************/

int ASSERTC(COND){
  if (!(COND)) return -1;
}

int ASSERT_EQ(COND1,COND2){
  if (COND1 != COND2) return -1;
}

/** Initialize buffers, etc.*/
void buffers_init(void)
{
	for(int i = 0; i < NUM_DATA_BUFFERS; i++)
	{
		data_buffers[i] = (uint8_t *) malloc(DATA_BUF_SIZE);
		if(data_buffers[i] == NULL)
		{
			//while(app_uart_put(67) != NRF_SUCCESS);
			printf("Data buffer %d Alloc failed\r\n", i);
		}
		memset(data_buffers[i], i+1, DATA_BUF_SIZE);
		comp_buffers_ready[i] = false;
		
		transmission_buffers[i] = (uint8_t *) malloc(TRANSMISSION_BUF_SIZE);
		if(transmission_buffers[i] == NULL)
		{
			printf("Transmission buffer %d Alloc failed\r\n", i);
		}
		memset(transmission_buffers[i], 0, TRANSMISSION_BUF_SIZE);
		tx_buffers_pending[i] = EMPTY_TX_BUFF_VAL;
		printf("txs:%d\r\n", tx_buffers_pending[i]);
	}
}

void compress(uint32_t buf, uint32_t size)
{
	  size_t count = 0;
    uint32_t sunk = 0;
    uint32_t polled = 0;
	
	  // Compression in progress.
	  //compression_in_progress = true;
	
	  //printf("Buf %d compression start...\r\n", buf);
	  // Clear encoder state machine
	  /*heatshrink_encoder_reset(&hse);
	
    while (sunk < size) {
        ASSERTC(heatshrink_encoder_sink(&hse, &(data_buffers[buf][sunk]), size - sunk, &count) >= 0);
        sunk += count;
        if (sunk == size) {
            ASSERT_EQ(HSER_FINISH_MORE, heatshrink_encoder_finish(&hse));
        }

        HSE_poll_res pres;
        do {                    //"turn the crank"
            pres = heatshrink_encoder_poll(&hse, &transmission_buffers[buf][polled], TRANSMISSION_BUF_SIZE - polled, &count);
            ASSERTC(pres >= 0);
            polled += count;
        } while (pres == HSER_POLL_MORE);
        ASSERT_EQ(HSER_POLL_EMPTY, pres);
        if (polled >= TRANSMISSION_BUF_SIZE) {
					printf("compression should never expand that muchr\r\n"); 
				}
        if (sunk == size) {
            ASSERT_EQ(HSER_FINISH_DONE, heatshrink_encoder_finish(&hse));
        }
    }
		
	// Finished compression.
	comp_buffers_ready[buf] = false;
	tx_buffers_pending[buf] = polled;
	//printf("txs1:%d\r\n", tx_buffers_pending[buf]);
	//printf("Buf %d compression done\r\n", buf);*/
	
	for(int i = 0; i < size; i++)
	{
		transmission_buffers[buf][i] = data_buffers[buf][i];
	}
	comp_buffers_ready[buf] = false;
	tx_buffers_pending[buf] = size ;
}

void sample_data(void)
{
	if(m_transfer_completed)
	{
		//if(intan_convert_channel < NUM_CHANNELS) {
		//	printf("%d\r\n", intan_convert_channel);
		//}
		m_transfer_completed = false;
		intan_convert(m_tx_data_spi, m_rx_data_spi,intan_convert_channel);
		intan_convert_channel++;
         
		switch_state();
	}
	
	// Previous transfer still in progress. Data lost
	else
	{
		while(app_uart_put(69) != NRF_SUCCESS);
	}
}

// Every time the spi timer expires, we begin data collection one channel at a time
static void spi_data_collection_evt_handler(void *pvParameter)
{ 
	//static int intan_convert_channel = 0;
	//if (m_transfer_completed)
  //{
	//while(app_uart_put(72) != NRF_SUCCESS);
	//m_transfer_completed = false;
  //}
	//while(app_uart_put(71) != NRF_SUCCESS);
	UNUSED_PARAMETER(pvParameter);
	//nrf_drv_gpiote_out_toggle(20);
	
	//printf("Ready to sample\r\n");
	sample_data();
	//while(app_uart_put(72) != NRF_SUCCESS);
}

// For testing; read out intan id
static void spi_intan_id_handler(void *pvParameter)
{
	static int company_title = 0;
	UNUSED_PARAMETER(pvParameter);
	if(m_transfer_completed)
	{
		//if(intan_convert_channel < NUM_CHANNELS) {
		//	printf("%d\r\n", intan_convert_channel);
		//}
		m_transfer_completed = false;
		intan_read(m_tx_data_spi, m_rx_data_spi,company_title + 40);
		company_title++;
		company_title = company_title % 5;
         
		switch_state();
	}
	
	// Previous transfer still in progress. Data lost
	else
	{
		while(app_uart_put(69) != NRF_SUCCESS);
	}
}

static void compress_task (void *pvParameter)
{
	// Continue to check if there is something to compress
	UNUSED_PARAMETER(pvParameter);
	for(;;)
	{
		
		//while(app_uart_put(70) != NRF_SUCCESS);
		//printf("compress task\r\n");
		if(comp_buffers_ready[comp_index])
		{
			//printf("cready%d\r\n", comp_index);
			
			// Check whether transmission buffer that the compressed data will be placed directly into is full or not.
			if(tx_buffers_pending[comp_index] == EMPTY_TX_BUFF_VAL)
			{
				compress(comp_index, DATA_BUF_SIZE);
				comp_index++;
				comp_index = comp_index % NUM_DATA_BUFFERS;
			}
			else
			{
				//printf("Still Txing, data lost.\r\n");
			}
		}
		
		// Transmit if transmission buffer has received a full set of data.
		if(tx_buffers_pending[tx_index] > 0 && !comp_buffers_ready[tx_index] && !tx_in_progress)
		{
			//printf("tready%d\r\n%d\r\n%d\r\n%d\r\n%d\r\n", tx_index, comp_buffers_ready[tx_index], comp_index, comp_buffers_ready[comp_index], tx_buffers_pending[tx_index]);
			tx_in_progress = true;
			curr_tx_size = tx_buffers_pending[tx_index];
			data_send(tx_index, tx_buffers_pending[tx_index]);
		}
		//vTaskDelay(COMP_DELAY);
	}
}
/*
void compression_evt_handler(void *p_context)
{
	uint32_t err_code;
	while(app_uart_put(74) != NRF_SUCCESS);
	compress(*((uint32_t *) p_context), DATA_BUF_SIZE);
	err_code = app_timer_stop(comp_timer);
	while(app_uart_put(75) != NRF_SUCCESS);
	APP_ERROR_CHECK(err_code);
}

void timers_init(void)
{
	uint32_t err_code;
	
	//APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
	err_code = app_timer_create(&spi_timer, APP_TIMER_MODE_REPEATED, spi_data_collection_evt_handler);
	APP_ERROR_CHECK(err_code);
	
	err_code = app_timer_create(&comp_timer, APP_TIMER_MODE_SINGLE_SHOT, compression_evt_handler);
	APP_ERROR_CHECK(err_code);
	//printf("%d", SAMPLE_DELAY);
	//printf("%d\n", err_code);
}

void data_collection_timers_start(void)
{
	uint32_t err_code;
	err_code = app_timer_start(spi_timer, SAMPLE_DELAY, NULL);
	//printf("%d\n", SAMPLE_DELAY);
	APP_ERROR_CHECK(err_code);
}*/

/**@brief Function for placing the application in low power state while waiting for events.
 */
/*static void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}*/

static void test_callback (void *pvParameter)
{
    static uint32_t counter = 0;
	  if(!(++counter%1000)) {
			printf("counter: %d\r\n", counter);
		}
}

/********************************** BLE TRANSMIT FUNCTIONS ************************************************/

/**@brief Function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num    Line number of the failing ASSERT call.
 * @param[in] p_file_name File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(0xDEADBEEF, line_num, p_file_name);
}


/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing services that will be used by the application.
 *
 * @details Initialize the multilink custom service.
 */
static void services_init(void)
{
    uint32_t            err_code;
    ble_uuid_t          uuid;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr;
    ble_gatts_attr_md_t attr_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_md_t char_ud_md;
    uint16_t            svc_test;

    static uint8_t multilink_peripheral_data;
    static uint8_t multilink_peripheral_ud[] = "MULTILINK DATA!!!!!!";

    ble_uuid128_t base_uuid = MULTILINK_PERIPHERAL_BASE_UUID;

    err_code = sd_ble_uuid_vs_add(&base_uuid, &m_base_uuid_type);
    APP_ERROR_CHECK(err_code);

    uuid.type = m_base_uuid_type;
    uuid.uuid = MULTILINK_PERIPHERAL_SERVICE_UUID;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &uuid, &svc_test);
    APP_ERROR_CHECK(err_code);

    uuid.uuid = MULTILINK_PERIPHERAL_CHAR_UUID;

    memset(&attr, 0, sizeof(ble_gatts_attr_t));
    attr.p_uuid    = &uuid;
    attr.p_attr_md = &attr_md;
    attr.max_len = MAX_DATA_LENGTH; //attr.max_len   = 1;
		attr.init_offs = 0;
    attr.p_value   = &multilink_peripheral_data;
    attr.init_len  = sizeof(multilink_peripheral_data);

    memset(&attr_md, 0, sizeof(ble_gatts_attr_md_t));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&attr_md.write_perm);
    attr_md.vloc = BLE_GATTS_VLOC_STACK;
    attr_md.vlen = 0;

    memset(&cccd_md, 0, sizeof(ble_gatts_attr_md_t));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&cccd_md.write_perm);
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(ble_gatts_char_md_t));
		//char_md.p_sccd_md         = NULL;
		//char_md.p_char_pf         = NULL;
    char_md.p_cccd_md               = &cccd_md;
    char_md.char_props.notify       = 1;
    char_md.char_props.indicate     = 1;
    char_md.char_props.read         = 1;
    char_md.char_props.write        = 1;
		char_md.char_props.write_wo_resp = 1;
    char_md.char_ext_props.wr_aux   = 1;
    char_md.p_user_desc_md          = &char_ud_md;
    char_md.p_char_user_desc        = multilink_peripheral_ud;
    char_md.char_user_desc_size     = (uint8_t)strlen((char *)multilink_peripheral_ud);
    char_md.char_user_desc_max_size = (uint8_t)strlen((char *)multilink_peripheral_ud);

    memset(&char_ud_md, 0, sizeof(ble_gatts_attr_md_t));
    char_ud_md.vloc = BLE_GATTS_VLOC_STACK;
    char_ud_md.vlen = 1;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&char_ud_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&char_ud_md.write_perm);

    err_code = sd_ble_gatts_characteristic_add(BLE_GATT_HANDLE_INVALID,
                                               &char_md,
                                               &attr,
                                               &m_char_handles);
    APP_ERROR_CHECK(err_code);
}

/**@brief   Function for handling app_uart events.
 *
 * @details This function will receive a single character from the app_uart module and append it to 
 *          a string. The string will be be sent over BLE when the last character received was a 
 *          'new line' i.e '\n' (hex 0x0D) or if the string has reached a length of 
 *          @ref NUS_MAX_DATA_LENGTH.
 */
void uart_event_handle(app_uart_evt_t * p_event)
{
    static uint8_t data_array[MAX_DATA_LENGTH];
    static uint8_t index = 0;

    switch (p_event->evt_type)
    {
        /**@snippet [Handling data from UART] */ 
        case APP_UART_DATA_READY:
            UNUSED_VARIABLE(app_uart_get(&data_array[index]));
            index++;

/*
            if ((data_array[index - 1] == '\n') || (index >= (BLE_NUS_MAX_DATA_LEN)))
            {
                while (ble_nus_c_string_send(&m_ble_nus_c, data_array, index) != NRF_SUCCESS)			
                {
                    // repeat until sent
                }
                index = 0;
            }
				*/
            break;
        /**@snippet [Handling data from UART] */ 
        case APP_UART_COMMUNICATION_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_communication);
            break;

        case APP_UART_FIFO_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_code);
            break;

        default:
            break;
    }
}

/**@brief Function for initializing the UART.
 */
static void uart_init(void)
{
    uint32_t err_code;

    const app_uart_comm_params_t comm_params =
      {
        .rx_pin_no    = RX_PIN_NUMBER,
        .tx_pin_no    = TX_PIN_NUMBER,
        .rts_pin_no   = RTS_PIN_NUMBER,
        .cts_pin_no   = CTS_PIN_NUMBER,
        .flow_control = APP_UART_FLOW_CONTROL_ENABLED,
        .use_parity   = false,
        .baud_rate    = UART_BAUDRATE_BAUDRATE_Baud38400
      };

    APP_UART_FIFO_INIT(&comm_params,
                        UART_RX_BUF_SIZE,
                        UART_TX_BUF_SIZE,
                        uart_event_handle,
                        APP_IRQ_PRIORITY_LOW,
                        err_code);

    APP_ERROR_CHECK(err_code);
}

/**
* Send one packet of data
* Input:
* - conn_handle: Handle ID of the connection.
* - char_handles: GATT characteristic handles
* - data: data to transmit (maximum number of bytes is MAX_DATA_LENGTH)
*/
uint32_t ble_multi_data_send(uint16_t conn_handle, ble_gatts_char_handles_t* char_handles, uint8_t* data, uint16_t length)
{
	/*ble_gattc_write_params_t gattc_params;
	
	// Verify parameters
	//VERIFY_PARAM_NOT_NULL(conn_handle);
	VERIFY_PARAM_NOT_NULL(char_handles);
	if(conn_handle == BLE_CONN_HANDLE_INVALID)
	{
		return NRF_ERROR_INVALID_STATE;
	}
	if(length > MAX_DATA_LENGTH)
	{
		return NRF_ERROR_INVALID_PARAM;
	}
	
	// Send data
	memset(&gattc_params, 0, sizeof(ble_gattc_write_params_t));
	
	gattc_params.handle = char_handles->value_handle;
  gattc_params.write_op   = BLE_GATT_OP_WRITE_CMD;
  gattc_params.offset = 0;
  gattc_params.len  = length;
  gattc_params.p_value = data;
	gattc_params.flags = BLE_GATT_EXEC_WRITE_FLAG_PREPARED_WRITE;
	
  return sd_ble_gattc_write(conn_handle, &gattc_params);*/
	
	ble_gatts_hvx_params_t hvx_params;
	uint32_t return_val;
	
	// Verify parameters
	//VERIFY_PARAM_NOT_NULL(conn_handle);
	VERIFY_PARAM_NOT_NULL(char_handles);
	if(conn_handle == BLE_CONN_HANDLE_INVALID)
	{
		return NRF_ERROR_INVALID_STATE;
	}
	if(length > MAX_DATA_LENGTH)
	{
		return NRF_ERROR_INVALID_PARAM;
	}
	
	// Send data
	memset(&hvx_params, 0, sizeof(ble_gatts_hvx_params_t));
	
	hvx_params.handle = char_handles->value_handle;
  hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
  hvx_params.offset = 0;
  hvx_params.p_len  = &length;
  hvx_params.p_data = data;
	
	return_val = sd_ble_gatts_hvx(conn_handle, &hvx_params);
	
  return return_val;
}

// Send the delimiter, where the final two bytes are used to represent the size of the buffer to send over.
static void send_delimeter(uint8_t buf, uint32_t size)
{
	// Last two bits have the size of the buffer just sent over in Big Endian order
	buf_delimiter[MAX_DATA_LENGTH - 2] = (size >> 8) & 0xFF;
	buf_delimiter[MAX_DATA_LENGTH - 1] = (size) & 0xFF;
	printf("%d\r\n", size);
	
	//sprintf(t_length, "%04d", curr_tx_size);
	//printf("hi");
	//buf_delimiter[MAX_DATA_LENGTH - 5] = 0;
  //buf_delimiter[MAX_DATA_LENGTH - 4] = 1;
	//buf_delimiter[MAX_DATA_LENGTH - 3] = 0;
	//buf_delimiter[MAX_DATA_LENGTH - 2] = 0;
	//buf_delimiter[MAX_DATA_LENGTH - 1] = 0;
	tx_buffers_pending[buf] = SEND_DELIMETER;
	//printf("sd1\r\n");
	//printf("1\r\n");
	while(ble_multi_data_send(m_conn_handle, &m_char_handles, buf_delimiter, MAX_DATA_LENGTH) != NRF_SUCCESS);
	//printf("sd2\r\n");
}

/**
* Continuously send data until TX buffers are full
*/
static void data_send(uint8_t buf, uint32_t size)
{
	uint32_t err_code;
	uint32_t counter = 0;
	//static uint8_t data[MAX_DATA_LENGTH] = "TK Smells Bad!!!!!\r\n";
	
	if(size < MAX_DATA_LENGTH)
	{
		while(ble_multi_data_send(m_conn_handle, &m_char_handles, &(transmission_buffers[buf][curr_tx_size - size]), size) != NRF_SUCCESS);
		while(ble_multi_data_send(m_conn_handle, &m_char_handles, zeroes_packet, MAX_DATA_LENGTH - size) != NRF_SUCCESS);
		tx_buffers_pending[buf] = 0;
	}
	else
	{
		while(ble_multi_data_send(m_conn_handle, &m_char_handles, &(transmission_buffers[buf][curr_tx_size - size]), MAX_DATA_LENGTH) != NRF_SUCCESS);
		tx_buffers_pending[buf] = tx_buffers_pending[buf] - MAX_DATA_LENGTH;
	}
	
	/*while(1)
	{
		err_code = ble_multi_data_send(m_conn_handle, &m_char_handles, data, 20);
		//printf("err_code dec: %d\r\n", err_code);
		//printf("%x\r\n", err_code);
		
		// Check for errors
		if(err_code != NRF_SUCCESS && 
			 err_code != BLE_ERROR_INVALID_CONN_HANDLE && 
		   err_code != NRF_ERROR_INVALID_STATE && 
		   err_code != BLE_ERROR_NO_TX_PACKETS &&
		   err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
		{
			printf("err_code dec: %d\r\n", err_code);
			printf("err_code hex: %x\r\n", err_code);
			APP_ERROR_CHECK(err_code);
		}
		
		// Transmission failed, usually meaning no more packets in this connection interval, so we exit and wait for the next interval
		if(err_code != NRF_SUCCESS)
		{
			if(err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
			{
				if(err_code == BLE_ERROR_NO_TX_PACKETS)
				{
					clear_to_send = false;
				}
				else
				{
					printf("%x\r\n", err_code);
				}
			}
			break;
		}
	}*/
}

/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void sleep_mode_enter(void)
{
    uint32_t err_code; // = bsp_indication_set(BSP_INDICATE_IDLE);
	  //err_code = bsp_indication_set(BSP_INDICATE_IDLE);
    //APP_ERROR_CHECK(err_code);

    // Prepare wakeup buttons.
    //err_code = bsp_btn_ble_sleep_mode_prepare();
    //APP_ERROR_CHECK(err_code);

    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    uint32_t err_code;

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            //err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
            //APP_ERROR_CHECK(err_code);
            break;

        case BLE_ADV_EVT_IDLE:
            sleep_mode_enter();
            break;

        default:
            break;
    }
}


/**@brief Function for handling the Application's BLE Stack events.
 *
 * @param[in] p_ble_evt   Bluetooth stack event.
 */
static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    uint32_t err_code;

    switch (p_ble_evt->header.evt_id)
    {
			  // Send more data once previous data is sent
			  case BLE_EVT_TX_COMPLETE:
					  //printf("txcomp\r\n");
				    if(ble_initialized)
						{
							// One transmission buffer and delimeter successfully completely sent.
							if(tx_buffers_pending[tx_index] == SEND_DELIMETER)
							{
								tx_buffers_pending[tx_index] = EMPTY_TX_BUFF_VAL;
								buf_delimiter[MAX_DATA_LENGTH - 2] = 78;
								buf_delimiter[MAX_DATA_LENGTH - 1] = 68;
								tx_in_progress = false;
								tx_index++;
								tx_index = tx_index % NUM_DATA_BUFFERS;
								//printf("Datasent\r\n");
							}
						
							// Transmission buffer completely sent; must still send the delimeter
							else if(tx_buffers_pending[tx_index] == 0)
							{
								send_delimeter(tx_index, curr_tx_size);
							}
						
							// Otherwise keep sending
							else
							{
								data_send(tx_index, tx_buffers_pending[tx_index]);
							}
						
							clear_to_send = true;
						}
						else
						{
							ble_initialized = true;
						}
				    break;
					  
        case BLE_GAP_EVT_CONNECTED:
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            //err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
            //APP_ERROR_CHECK(err_code);
				
				    // Missing gatt sys attributes
				    //err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, BLE_GATTS_SYS_ATTR_FLAG_SYS_SRVCS | BLE_GATTS_SYS_ATTR_FLAG_USR_SRVCS);
				    //APP_ERROR_CHECK(err_code);
				    
				    // Start sending data once connected
				    printf("Central Connected \r\n");
						//err_code = app_timer_start(ble_init_timer_id, BLE_INIT_PERIOD, NULL);
				    //APP_ERROR_CHECK(err_code);
				    //while(ble_multi_data_send(m_conn_handle, &m_char_handles, zeroes_packet, MAX_DATA_LENGTH) != NRF_SUCCESS);
				    clear_to_send = true;
				    connected = true;
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            //err_code = bsp_indication_set(BSP_INDICATE_IDLE);
            //APP_ERROR_CHECK(err_code);

            //err_code = bsp_indication_set(BSP_INDICATE_ALERT_OFF);
            //APP_ERROR_CHECK(err_code);
				    vTaskSuspendAll();   // After disconnecting, we just want to pause and retain the timers.
            break;

        case BLE_GATTS_EVT_WRITE:
            if ((p_ble_evt->evt.gatts_evt.params.write.handle == m_char_handles.cccd_handle) &&
                (p_ble_evt->evt.gatts_evt.params.write.len == 2))
            {
                //err_code = bsp_indication_set(BSP_INDICATE_RCV_OK);
                //APP_ERROR_CHECK(err_code);
            }
            break;
						
				case BLE_GATTS_EVT_SYS_ATTR_MISSING:
						// Deal with missing gatt system attributes
				    err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
				    printf("SYS attr missing error: %d\r\n", err_code);
				    //APP_ERROR_CHECK(err_code);
				    break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the scheduler in the main loop after a BLE stack
 *          event has been received.
 *
 * @param[in] p_ble_evt   Bluetooth stack event.
 */
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    dm_ble_evt_handler(p_ble_evt);
    //bsp_btn_ble_on_ble_evt(p_ble_evt);
    on_ble_evt(p_ble_evt);
    ble_advertising_on_ble_evt(p_ble_evt);
}


/**@brief Function for dispatching a system event to interested modules.
 *
 * @details This function is called from the System event interrupt handler after a system
 *          event has been received.
 *
 * @param[in]   sys_evt   System stack event.
 */
static void sys_evt_dispatch(uint32_t sys_evt)
{
    pstorage_sys_event_handler(sys_evt);
    ble_advertising_on_sys_evt(sys_evt);
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    uint32_t err_code;

    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM, NULL);
    
    ble_enable_params_t ble_enable_params;
    err_code = softdevice_enable_get_default_config(CENTRAL_LINK_COUNT,
                                                    PERIPHERAL_LINK_COUNT,
                                                    &ble_enable_params);
    APP_ERROR_CHECK(err_code);
    
    //Check the ram settings against the used number of links
    CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT,PERIPHERAL_LINK_COUNT);
    
    // Enable BLE stack.
    err_code = softdevice_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for BLE events.
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for system events.
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);

}


/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated by button press.
 */
/*void bsp_event_handler(bsp_event_t event)
{
    uint32_t err_code;
    switch (event)
    {
        case BSP_EVENT_SLEEP:
            sleep_mode_enter();
            break;

        case BSP_EVENT_DISCONNECT:
            err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
            break;

        case BSP_EVENT_KEY_0:
            if (m_conn_handle != BLE_CONN_HANDLE_INVALID)
            {
                static uint8_t value = 0;

                ble_gatts_hvx_params_t hvx_params;
                uint16_t               len = sizeof(uint8_t);

                value = (value == 0) ? 1 : 0;

                memset(&hvx_params, 0, sizeof(hvx_params));

                hvx_params.handle = m_char_handles.value_handle;
                hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
                hvx_params.offset = 0;
                hvx_params.p_len  = &len;
                hvx_params.p_data = &value;

                err_code = sd_ble_gatts_hvx(m_conn_handle, &hvx_params);
                APP_ERROR_CHECK(err_code);

                if (value == 0)
                {
                    err_code = bsp_indication_set(BSP_INDICATE_ALERT_OFF);
                    APP_ERROR_CHECK(err_code);
                }
                else
                {
                    err_code = bsp_indication_set(BSP_INDICATE_ALERT_3);
                    APP_ERROR_CHECK(err_code);
                }
            }
            break;

        default:
            break;
    }
}*/


/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void)
{
    uint32_t      err_code;
    ble_advdata_t advdata;

    // Build and set advertising data
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance      = true;
    advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

    ble_adv_modes_config_t options = {0};
    options.ble_adv_fast_enabled  = BLE_ADV_FAST_ENABLED;
    options.ble_adv_fast_interval = APP_ADV_INTERVAL;
    options.ble_adv_fast_timeout  = APP_ADV_TIMEOUT_IN_SECONDS;

    err_code = ble_advertising_init(&advdata, NULL, &options, on_adv_evt, NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the Bond Manager events.
 *
 * @param[in]   p_evt   Data associated to the bond manager event.
 */
static uint32_t device_manager_evt_handler(dm_handle_t const * p_handle,
                                           dm_event_t const  * p_event,
                                           ret_code_t        event_result)
{   uint32_t err_code;

    switch(p_event->event_id)
    {
        case DM_EVT_CONNECTION:
            m_conn_handle = p_event->event_param.p_gap_param->conn_handle;
            //err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
            //APP_ERROR_CHECK(err_code);
            break;

        case DM_EVT_DISCONNECTION:
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            break;
    }

    return NRF_SUCCESS;
}


/**@brief Function for the Device Manager initialization.
 *
 * @param[in] erase_bonds  Indicates whether bonding information should be cleared from
 *                         persistent storage during initialization of the Device Manager.
 */
static void device_manager_init(bool erase_bonds)
{
    uint32_t               err_code;
    dm_init_param_t        init_param = {.clear_persistent_data = erase_bonds};
    dm_application_param_t register_param;

    // Initialize persistent storage module.
    err_code = pstorage_init();
    APP_ERROR_CHECK(err_code);

    err_code = dm_init(&init_param);
    APP_ERROR_CHECK(err_code);

    register_param.sec_param.bond         = SEC_PARAM_BOND;
    register_param.sec_param.mitm         = SEC_PARAM_MITM;
    register_param.sec_param.io_caps      = SEC_PARAM_IO_CAPABILITIES;
    register_param.sec_param.oob          = SEC_PARAM_OOB;
    register_param.sec_param.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
    register_param.sec_param.max_key_size = SEC_PARAM_MAX_KEY_SIZE;
    register_param.evt_handler            = device_manager_evt_handler;
    register_param.service_type           = DM_PROTOCOL_CNTXT_GATT_SRVR_ID;

    err_code = dm_register(&m_app_handle, &register_param);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing buttons and leds.
 *
 * @param[out] p_erase_bonds  Will be true if the clear bonding button was pressed to wake the application up.
 */
/*static void buttons_leds_init(bool * p_erase_bonds)
{
    bsp_event_t startup_event;

    uint32_t err_code = bsp_init(BSP_INIT_LED | BSP_INIT_BUTTONS,
                                 APP_TIMER_TICKS(100, APP_TIMER_PRESCALER),
                                 bsp_event_handler);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_btn_ble_init(NULL, &startup_event);
    APP_ERROR_CHECK(err_code);

    *p_erase_bonds = (startup_event == BSP_EVENT_CLEAR_BONDING_DATA);
}*/

// Keep trying to establish connection.
static void ble_init_handler(void* p_context)
{
	//UNUSED_PARAM(p_context);
	ble_multi_data_send(m_conn_handle, &m_char_handles, zeroes_packet, MAX_DATA_LENGTH);
}


/**@brief Function for the Power manager.
 */
static void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for application main entry.
 */
int main(void)
{
	  TaskHandle_t  compress_task_handle;
	  TimerHandle_t spi_timer_handle;
    uint32_t err_code;
    bool erase_bonds;

    // Initialize.
    //APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, NULL);
	  uart_init();
	  printf("uart init\r\n");
    //buttons_leds_init(&erase_bonds);
    ble_stack_init();
	  printf("ble init\r\n");
    device_manager_init(erase_bonds);
	  printf("device manager init\r\n");
    gap_params_init();
	  printf("gap init\r\n");
    advertising_init();
	  printf("advertising init\r\n");
    services_init();
	  printf("services init\r\n");

		intan_setup();
		while(app_uart_put(65) != NRF_SUCCESS);
			
		buffers_init();
		while(app_uart_put(66) != NRF_SUCCESS);
			
	  //printf("yo\n");
		UNUSED_VARIABLE(xTaskCreate(compress_task, "c_task", configMINIMAL_STACK_SIZE + 200, NULL, 1, &compress_task_handle));
			
		if(compress_task_handle == NULL)
		{
			printf("compression task init messed up\r\n");
		}
		while(app_uart_put(67) != NRF_SUCCESS);
		
		//Test read intan company ID timer
		//spi_timer_handle = xTimerCreate("s_timer", 1, pdTRUE, NULL, spi_intan_id_handler);
		
		// Sample timer
    spi_timer_handle = xTimerCreate("s_timer", 10, pdTRUE, NULL, spi_data_collection_evt_handler);                                 // LED1 timer creation
    
		if(spi_timer_handle == NULL)
		{
			printf("SPI timer init messed up\r\n");
		}
		while(app_uart_put(68) != NRF_SUCCESS);
		
		if(xTimerStart(spi_timer_handle, 0) != pdPASS)
		{
			printf("Timer could not be started\r\n");
		}
		while(app_uart_put(69) != NRF_SUCCESS);
		
		//err_code = app_timer_create(&ble_init_timer_id, APP_TIMER_MODE_REPEATED, ble_init_handler);
		//APP_ERROR_CHECK(err_code);

    err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
	  printf("main_advertising start init\r\n");
    APP_ERROR_CHECK(err_code);

    for (;;)
    {
			if(connected && !ble_initialized && !first_tx_successful)
			{
				err_code = ble_multi_data_send(m_conn_handle, &m_char_handles, buf_delimiter, MAX_DATA_LENGTH);
				if(err_code == NRF_SUCCESS)
				{
					first_tx_successful = true;
					nrf_delay_ms(5000);
				}
				else
				{
					nrf_delay_ms(DELAY_MS);
				}
			}
			else
			{
				if(connected && !rtos_running)
				{
					printf("begin meas\r\n");
					//ble_multi_data_send(m_conn_handle, &m_char_handles, "TK_SMELLS BAD!!!!!\r\n", 20);
					//data_send();
					//clear_to_send = false;
					//printf("Data sent \r\n");
					// Activate deep sleep mode
					rtos_running = true;
					SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

					// Start FreeRTOS scheduler. This case is where a connection has not been established previously, so rtos and the timers are started.
					vTaskStartScheduler();
				}
				else if(connected)
				{
					// Resume task if it has only been suspended (a connection has been established previously, so we are just resuming the timers.)
					xTaskResumeAll();
				}
			}
		}
		power_manage();
}


//Test main
/*int main(void)
{
	  TaskHandle_t  compress_task_handle;
	  TimerHandle_t spi_timer_handle;
	  uint32_t err_code;
	
	  //err_code = nrf_drv_clock_init();
    //APP_ERROR_CHECK(err_code);
	
    // Setup bsp module.
    bsp_configuration();

    //uart initialization
    const app_uart_comm_params_t comm_params =
      {
          RX_PIN_NUMBER,
          TX_PIN_NUMBER,
          RTS_PIN_NUMBER,
          CTS_PIN_NUMBER,
          APP_UART_FLOW_CONTROL_ENABLED,
          false,
          UART_BAUDRATE_BAUDRATE_Baud38400
      };

    APP_UART_FIFO_INIT(&comm_params,
                         UART_RX_BUF_SIZE,
                         UART_TX_BUF_SIZE,
                         uart_error_handle,
                         APP_IRQ_PRIORITY_LOW,
                         err_code);

    APP_ERROR_CHECK(err_code);
			
		//nrf_drv_gpiote_out_config_t config = GPIOTE_CONFIG_OUT_TASK_HIGH;
		//config.init_state = true;
		//err_code = nrf_drv_gpiote_out_init(20, &config);

		//while(app_uart_put(65) != NRF_SUCCESS);
		//printf("1 ms = %d\n", pdMS_TO_TICKS(1));
		intan_setup();
		//while(app_uart_put(65) != NRF_SUCCESS);
			
		compression_init();
		while(app_uart_put(66) != NRF_SUCCESS);
			
	  //printf("yo\n");
		UNUSED_VARIABLE(xTaskCreate(compress_task, "c_task", configMINIMAL_STACK_SIZE + 200, NULL, 1, &compress_task_handle));
			
		if(compress_task_handle == NULL)
		{
			printf("compression task init messed up\r\n");
		}
		while(app_uart_put(67) != NRF_SUCCESS);
		
		//Test read intan company ID timer
		//spi_timer_handle = xTimerCreate("s_timer", 1, pdTRUE, NULL, spi_intan_id_handler);
		
		// Sample timer
    spi_timer_handle = xTimerCreate("s_timer", 1, pdTRUE, NULL, spi_data_collection_evt_handler);                                 // LED1 timer creation
    
		if(spi_timer_handle == NULL)
		{
			printf("SPI timer init messed up\r\n");
		}
		while(app_uart_put(68) != NRF_SUCCESS);
		
		if(xTimerStart(spi_timer_handle, 0) != pdPASS)
		{
			printf("Timer could not be started\r\n");
		}
		while(app_uart_put(69) != NRF_SUCCESS);
			
		//Activate deep sleep mode
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    // Start FreeRTOS scheduler.
    vTaskStartScheduler();
			
		//timers_init();
		//while(app_uart_put(67) != NRF_SUCCESS);
			
		//data_collection_timers_start();
		//while(app_uart_put(68) != NRF_SUCCESS);

    for (;;)
    {
			  printf("Messed up\r\n");
			  //power_manage();*/
        /*if (m_transfer_completed)
        {
					  //while(app_uart_put(72) != NRF_SUCCESS);
            m_transfer_completed = false;

            intan_convert(m_tx_data_spi, m_rx_data_spi,intan_convert_channel);
					  //while(app_uart_put(73) != NRF_SUCCESS);
            intan_convert_channel ++;
            intan_convert_channel = intan_convert_channel % 32;
            //print m_rx_data_spi results
            switch_state();
					  //while(app_uart_put(74) != NRF_SUCCESS);
					  //for (int i; i< RX_MSG_LENGTH; i++){
						//	while(app_uart_put(m_rx_data_spi[i]) != NRF_SUCCESS);
						//}
            nrf_delay_ms(DELAY_MS);
						//while(app_uart_put(75) != NRF_SUCCESS);
        }*/
				//while(app_uart_put(76) != NRF_SUCCESS);
    //}
//}
