/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
* @defgroup spi_master_example_main main.c
* @{
* @ingroup spi_master_example
*
* @brief SPI Master Loopback Example Application main file.
*
* This file contains the source code for a sample application using SPI.
*
*/

#include <stdlib.h>
#include <string.h>

#include "nrf_delay.h"
#include "app_error.h"
#include "app_util_platform.h"
#include "app_uart.h"
#include "nrf_drv_spi.h"
#include "bsp.h"
#include "app_timer.h"
#include "nordic_common.h"
#include "nrf.h"
#include "ble_data_buffer.h"

/********************************** DATA ACQUISITION DEFINITIONS ************************************************/

#define APP_TIMER_PRESCALER      0                      ///< Value of the RTC1 PRESCALER register.
#define APP_TIMER_OP_QUEUE_SIZE  2                      ///< Size of timer operation queues.


#define DELAY_MS                 1 /*1000*/                   ///< Timer Delay in milli-seconds.

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

#define UART_TX_BUF_SIZE 2                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 2                           /**< UART RX buffer size. */

typedef enum
{
    TEST_STATE_SPI0_LSB,    ///< Test SPI0, bits order LSB
    TEST_STATE_SPI0_MSB,    ///< Test SPI0, bits order MSB
    END_OF_TEST_SEQUENCE
} spi_master_ex_state_t;

typedef enum
{
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
//static spi_master_ex_state_t m_spi_master_ex_state = (spi_master_ex_state_t)0;

static const nrf_drv_spi_t m_spi_master_0 = NRF_DRV_SPI_INSTANCE(0);

/********************************** COMPRESSION DEFINITIONS ************************************************/

#define NUM_DATA_BUFFERS 2                                                     /**<Number of data buffers to use> */
#define DATA_BUF_SIZE 	(2048U)                                                /**<buffer size for each data buffer.> */
#define TRANSMISSION_BUF_SIZE 	(DATA_BUF_SIZE + (DATA_BUF_SIZE / 2) + 4)          /**<buffer size for the transmission buffer.> */

static data_buffer db = {0};
static int intan_convert_channel = 0;

static uint8_t *transmission_buffer;                                           /**<Buffer for storing and sending the compressed data>*/
static heatshrink_encoder hse;

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
void *spi_master_0_event_handler_implementation(nrf_drv_spi_evt_type_t* event);
static void spi_master_init(nrf_drv_spi_t const * p_instance, bool lsb);
static void spi_send_recv(nrf_drv_spi_t const * p_instance,
                          uint8_t * p_tx_data,
                          uint8_t * p_rx_data,
                          uint16_t  tx_len,
                          uint16_t  rx_len);
static void switch_state(void);
void bsp_configuration(void);
void intan_setup(void);


/********************************** COMPRESSION PROTOTYPE ************************************************/
void compression_init(void);
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
    UNUSED_VARIABLE(bsp_indication_set(BSP_INDICATE_FATAL_ERROR));
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
//    uint32_t err_code = NRF_SUCCESS;
//	  uint32_t c_buff;   // Temporarily store the buffer index of the buffer ready to be compressed

    switch (*event)
    {
			  // Transfer complete; store in buffer.
        case NRF_DRV_SPI_EVENT_DONE:
            
            nrf_drv_spi_uninit(&m_spi_master_0);
            if (buffer_in(&db, &m_rx_data_spi[0]) == BUFFER_FULL) {
              if (buffer_compress(db.buffer, db.item_cnt, transmission_buffer) != BUFFER_SUCCESS) {
             //   buffer_reset(&db);  
								printf("Error compressing data!\n");
              }
              intan_convert_channel++;
              intan_convert_channel = intan_convert_channel % 32;
						//	printf("Now the channel num is %d", intan_convert_channel);
						//		while(app_uart_put(intan_convert_channel + 65) != NRF_SUCCESS);
						}

            m_transfer_completed = true;
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
        .frequency    = NRF_DRV_SPI_FREQ_1M,
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
            spi_master_0_event_handler);
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

    NRF_CLOCK->LFCLKSRC            = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_LFCLKSTART    = 1;

    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0)
    {
        // Do nothing.
    }

    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, NULL);

    err_code = bsp_init(BSP_INIT_LED, APP_TIMER_TICKS(100, APP_TIMER_PRESCALER), NULL);
    APP_ERROR_CHECK(err_code);
}

void intan_setup(void){
	int setup_reg[13] = {1,2,4,8,9,10,11,12,13,14,15,16,17};
	int setup_data[13] = {ADC_bias & 0x3F, MUX_bias, ADC_twoscomp, RH1_DAC1,RH1_DAC2,RH2_DAC1,RH2_DAC2,RL_DAC1,((RL_DAC3 << 6) + RL_DAC2),0xFF,0xFF,0xFF,0xFF};

	for (int i =0; i<13; i++){
		while (!m_transfer_completed){};

    	m_transfer_completed = false;
    	intan_write(m_tx_data_spi, m_rx_data_spi,setup_reg[i],setup_data[i]);
    	switch_state();
    	//nrf_delay_ms(DELAY_MS);
    	
    }
    for (int j =0; j<2; j++){
  		while (!m_transfer_completed){};
      	        
      	m_transfer_completed = false;
      	intan_dummy(m_tx_data_spi, m_rx_data_spi);
      	switch_state();
      	//nrf_delay_ms(DELAY_MS);
    }
}

/** @brief Function for main application entry.
 */
int main(void)
{
    // Setup bsp module.
    bsp_configuration();
	
    //uart initialization
    uint32_t err_code;
    const app_uart_comm_params_t comm_params =
      {
          RX_PIN_NUMBER,
          TX_PIN_NUMBER,
          RTS_PIN_NUMBER,
          CTS_PIN_NUMBER,
          APP_UART_FLOW_CONTROL_ENABLED,
          false,
          UART_BAUDRATE_BAUDRATE_Baud115200
      };

    APP_UART_FIFO_INIT(&comm_params,
                         UART_RX_BUF_SIZE,
                         UART_TX_BUF_SIZE,
                         uart_error_handle,
                         APP_IRQ_PRIORITY_LOW,
                         err_code);

		
    APP_ERROR_CHECK(err_code);

			
		while(app_uart_put(86) != NRF_SUCCESS);
		intan_setup();
			
	  while(app_uart_put(87) != NRF_SUCCESS);
    // initialize the buffer 
    buffer_init(&db, DATA_BUF_SIZE, sizeof(uint8_t));
			
		while(app_uart_put(88) != NRF_SUCCESS);
    for (;;)
    {
		//	printf("ldsakjf;ljdsaflkjlljlk\n");
 			while(app_uart_put(89) != NRF_SUCCESS);
			
        if (m_transfer_completed)
        {
            m_transfer_completed = false;

            intan_convert(m_tx_data_spi, m_rx_data_spi,intan_convert_channel);
	 
            //print m_rx_data_spi results
            switch_state();
					  for (int i; i< RX_MSG_LENGTH; i++){
							while(app_uart_put(m_rx_data_spi[i]) != NRF_SUCCESS);
						}
            nrf_delay_ms(DELAY_MS);
						//printf("Hi\n");
        }
				//printf("Yo\n");
    }
}

/** @} */
