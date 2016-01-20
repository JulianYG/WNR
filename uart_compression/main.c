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
 * @defgroup uart_example_main main.c
 * @{
 * @ingroup uart_example
 * @brief UART Example Application main file.
 *
 * This file contains the source code for a sample application using UART.
 * 
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "app_uart.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf.h"
#include "bsp.h"

#include "minilzo.h"
//#define ENABLE_LOOPBACK_TEST  /**< if defined, then this example will be a loopback test, which means that TX should be connected to RX to get data loopback. */

#define MAX_TEST_DATA_BYTES     (1155U)                /**< max number of test bytes to be used for tx and rx. */
#define UART_TX_BUF_SIZE 1024                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 1                         /**< UART RX buffer size. */

static unsigned char __LZO_MMODEL tx_data  [ UART_TX_BUF_SIZE ];
static unsigned char __LZO_MMODEL tx_data_comp [ 1155 ];

/* Work-memory needed for compression. Allocate memory in units
 * of 'lzo_align_t' (instead of 'char') to make sure it is properly aligned.
 */

#define HEAP_ALLOC(var,size) \
    lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]

static HEAP_ALLOC(wrkmem, LZO1X_1_MEM_COMPRESS);

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

#ifdef ENABLE_LOOPBACK_TEST
/** @brief Function for setting the @ref ERROR_PIN high, and then enter an infinite loop.
 */
static void show_error(void)
{   
    LEDS_ON(LEDS_MASK);
    while(true)
    {
        // Do nothing.
    }
}


/** @brief Function for testing UART loop back. 
 *  @details Transmitts one character at a time to check if the data received from the loopback is same as the transmitted data.
 *  @note  @ref TX_PIN_NUMBER must be connected to @ref RX_PIN_NUMBER)
 */
static void uart_loopback_test()
{
 //   uint8_t * tx_data = (uint8_t *)("\n\rLOOPBACK_TEST\n\r");
    uint8_t   rx_data;

    // Start sending one byte and see if you get the same
    for (uint32_t i = 0; i < MAX_TEST_DATA_BYTES; i++)
    {
        uint32_t err_code;

        int r;
        lzo_uint in_len;
        lzo_uint out_len;
        lzo_uint new_len;

        /*
         * Step 2: prepare the input block that will get compressed.
         * We just fill it with zeros in this example program,
         * but you would use your real-world data here.
         */
        in_len = UART_TX_BUF_SIZE;

        // not filling 0's here
        // lzo_memset(in,0,in_len);

        /*
         * Step 3: compress from 'in' to 'out' with LZO1X-1
         */
        r = lzo1x_1_compress(tx_data,in_len,tx_data_comp,&out_len,wrkmem);
        if (r == LZO_E_OK)
            printf("compressed %lu bytes into %lu bytes\n",
                (unsigned long) in_len, (unsigned long) out_len);
        else
        {
            /* this should NEVER happen */
            printf("internal error - compression failed: %d\n", r);
            return 2;
        }
        /* check for an incompressible block */
        if (out_len >= in_len)
        {
            printf("This block contains incompressible data.\n");
            return 0;
        }


        while(app_uart_put(tx_data_comp[i]) != NRF_SUCCESS);

        nrf_delay_ms(10);
        err_code = app_uart_get(&rx_data);

        if ((rx_data != tx_data_comp[i]) || (err_code != NRF_SUCCESS))
        {
            show_error();
        }
    }
    return;
}


#endif


/**
 * @brief Function for main application entry.
 */
int main(void)
{
    LEDS_CONFIGURE(LEDS_MASK);
    LEDS_OFF(LEDS_MASK);
    uint32_t err_code;
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

#ifndef ENABLE_LOOPBACK_TEST
    printf("\n\rStart: \n\r");

    while (true)
    {
        uint8_t cr;
        while(app_uart_get(&cr) != NRF_SUCCESS);
        while(app_uart_put(cr) != NRF_SUCCESS);

        if (cr == 'q' || cr == 'Q')
        {
            printf(" \n\rExit!\n\r");

            while (true)
            {
                // Do nothing.
            }
        }
    }
#else

    // This part of the example is just for testing the loopback .

    /*
     * Initialize the miniLZO library first
     */
    if (lzo_init() != LZO_E_OK)
    {
      printf("internal error - lzo_init() failed !!!\n");
      printf("(this usually indicates a compiler bug - try recompiling without optimizations, and enable '-DLZO_DEBUG' for diagnostics)\n");
      return 3;
    }

    while (true)
    {
        uart_loopback_test();
    }
#endif
}


/** @} */
