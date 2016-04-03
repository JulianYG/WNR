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
//#include "app_timer.h"
#include "nordic_common.h"
#include "nrf.h"

#include "heatshrink_encoder.h"
#include "heatshrink_decoder.h"
#include "heatshrink_config.h"
#include "micro_esb.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "nrf_drv_clock.h"
#include "nrf_soc.h"

#include "nrf_drv_gpiote.h"

/********************************** DATA ACQUISITION DEFINITIONS ************************************************/

#define APP_TIMER_PRESCALER      0                      ///< Value of the RTC1 PRESCALER register.
//#define APP_TIMER_MAX_TIMERS     BSP_APP_TIMERS_NUMBER  ///< Maximum number of simultaneously created timers.
#define APP_TIMER_OP_QUEUE_SIZE  2                      ///< Size of timer operation queues.
#define SAMPLE_DELAY APP_TIMER_TICKS(1, APP_TIMER_PRESCALER) // Sample rate in MS
#define NUM_CHANNELS 16                                         // Number of channels to sample from

#define DELAY_MS                 1000 /*1000*/                   ///< Timer Delay in milli-seconds.

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

#define UART_TX_BUF_SIZE 256                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 256                           /**< UART RX buffer size. */

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


static uint8_t *transmission_buffer;                                           /**<Buffer for storing and sending the compressed data>*/
static uint8_t *data_buffers[NUM_DATA_BUFFERS];                                /**<Buffers for storing data>*/
static bool buffers_ready[NUM_DATA_BUFFERS];                                   /**<At each index, stores whether or not the buffer is full of raw data>*/
static uint8_t active_buffer = 0;                                              /**<Buffer index that is currently storing incoming data>*/
static uint8_t comp_index = 0;                                                 /**<Buffer index that will be filled and compressed next>*/
static uint16_t ab_capacity = DATA_BUF_SIZE;                                   /**<How much more space is left in the active buffer>*/

//static bool compression_in_progress = false;                                   /**<Flag for whether or not is in progress>*/

static heatshrink_encoder hse;

//APP_TIMER_DEF(comp_timer);    // Timer for running compression asynchronously
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
				    if(buffers_ready[active_buffer])
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
				
						printf("%c\r\n",m_rx_data_spi[1]);
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

/** Initialize compression variables, buffers, etc.*/
void compression_init(void)
{
	transmission_buffer = malloc(TRANSMISSION_BUF_SIZE);
	if(transmission_buffer == NULL)
	{
		//while(app_uart_put(66) != NRF_SUCCESS);
		printf("Transmission buffer Alloc failed\r\n");
	}
	
	memset(transmission_buffer, 0, TRANSMISSION_BUF_SIZE);
	for(int i = 0; i < NUM_DATA_BUFFERS; i++)
	{
		data_buffers[i] = malloc(DATA_BUF_SIZE);
		
		if(data_buffers[i] == NULL)
		{
			//while(app_uart_put(67) != NRF_SUCCESS);
			printf("Data buffer %d Alloc failed\r\n", i);
		}

		memset(data_buffers[i], 0, DATA_BUF_SIZE);
		buffers_ready[i] = false;
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
	  heatshrink_encoder_reset(&hse);
	
    while (sunk < size) {
        ASSERTC(heatshrink_encoder_sink(&hse, &(data_buffers[buf][sunk]), size - sunk, &count) >= 0);
        sunk += count;
        if (sunk == size) {
            ASSERT_EQ(HSER_FINISH_MORE, heatshrink_encoder_finish(&hse));
        }

        HSE_poll_res pres;
        do {                    /* "turn the crank" */
            pres = heatshrink_encoder_poll(&hse, &transmission_buffer[polled], TRANSMISSION_BUF_SIZE - polled, &count);
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
	buffers_ready[buf] = false;
	printf("Buf %d compression done\r\n", buf);
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
		if(buffers_ready[comp_index])
		{
			printf("cready\r\n");
			compress(comp_index++, DATA_BUF_SIZE);
			comp_index = comp_index % NUM_DATA_BUFFERS;
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

/** @brief Function for main application entry.
 */
int main(void)
{
	  TaskHandle_t  compress_task_handle;
	  TimerHandle_t spi_timer_handle;
	  uint32_t err_code;
	
	  err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
	
    // Setup bsp module.
    //bsp_configuration();

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
			
		/* Activate deep sleep mode */
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
			  //power_manage();
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
    }
}

/** @} */
