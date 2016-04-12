
/**************************************************

file: demo_rx.c
purpose: simple demo that receives characters from
the serial port and print them on the screen,
exit the program by pressing Ctrl-C

compile with the command: gcc demo_rx.c rs232.c -Wall -Wextra -o2 -o test_rx

**************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "rs232.h"
#include "heatshrink_decoder.h"
#include "heatshrink_common.h"
#include "heatshrink_config.h"
#define MAX_DATA_SIZE 2048
static heatshrink_decoder hsd;
static void decompress(uint8_t *input, uint32_t input_size);
static int arr_search(uint8_t *toSearch, int lenSearch, uint8_t *arr, int lenArr);

int main(void)
{
    int i, n,
        cport_nr = 4,        /* /dev/ttyS0 (COM1 on windows) */
        bdrate = 38400;       /* 9600 baud */

    uint8_t buf[21];
    uint8_t data[MAX_DATA_SIZE];
    memset(data, 0, MAX_DATA_SIZE);	/* Initialize the array */

    char mode[] = {'8','N','1',0};

    if (RS232_OpenComport(cport_nr, bdrate, mode)) {
        printf("Can not open comport\n");
        return(0);
    }
    int received_cnt = 0;

    while (1) {
      	n = RS232_PollComport(cport_nr, buf, 20);

	    if (n > 0) {
		    buf[n] = 0;   /* always put a "null" at the end of a string! */
		    for (i = 0; i < n; ++i) {
		        if (buf[i] < 32) {  /* replace unreadable control-codes by dots */
		            buf[i] = '.';
		        }
		    }

		    printf("Received %i bytes: %s\n", n, (char *) buf);

		    if ((int pos = arr_search("BCDEF", 5, buf, n)) >= 0) {	/* If receiving the end of current compressed buffer, send & reinitialize */
		    	int eff_len = pos - 5;
		    	uint8_t temp[eff_len];
		    	memcpy(temp, buf, eff_len);
		    	memcpy(data + received_cnt, temp, eff_len);
		    	received_cnt += eff_len;
		    	/* Discard the last five bytes of indicators*/
		    	decompress(data, received_cnt);
		    	/* Clean up */
		    	memset(data, 0, MAX_DATA_SIZE);
		    	received_cnt = 0;	
		    	/* Also need to store rest of the data to avoid loss */
		    	uint8_t lost[n - eff_len - 5];
		    	memcpy(lost, buf + pos, n - pos);
		    	memcpy(data, lost, n - pos);
		    	received_cnt += n - pos;
		    } else {	/* If regular data packets, store it*/
		    	memcpy(data + received_cnt, buf, n);
		    	received_cnt = received_cnt + n;
		    }
    	}
		#ifdef _WIN32
		    Sleep(100);
		#else
		    usleep(100000);  /* sleep for 100 milliSeconds */
		#endif
	}

    return (0);
}

 void decompress(uint8_t *input, uint32_t compressed_size)
 {
 	size_t decomp_sz = 2048;
	uint8_t *decomp = malloc(decomp_sz);

	memset(decomp, 0, decomp_sz);
	size_t count = 0;
	uint32_t sunk = 0;
    uint32_t polled = 0;

    while (sunk < compressed_size) {
        ASSERT(heatshrink_decoder_sink(&hsd, &input[sunk], compressed_size - sunk, &count) >= 0);
        sunk += count;
        if (sunk == compressed_size) {
            ASSERT_EQ(HSDR_FINISH_MORE, heatshrink_decoder_finish(&hsd));
        }
        HSD_poll_res pres;
        do {
            pres = heatshrink_decoder_poll(&hsd, &decomp[polled],
                decomp_sz - polled, &count);
            ASSERT(pres >= 0);
            polled += count;
        } while (pres == HSDR_POLL_MORE);
        ASSERT_EQ(HSDR_POLL_EMPTY, pres);
        if (sunk == compressed_size) {
            HSD_finish_res fres = heatshrink_decoder_finish(&hsd);
            ASSERT_EQ(HSDR_FINISH_DONE, fres);
        }
    }
    for (int i = 0; i < polled; ++i) {
    	printf("%hhu", decomp[i]);
    }
	free(decomp);
 }

int arr_search(uint8_t *toSearch, int lenSearch, uint8_t *arr, int lenArr)
{
	int pos_search = 0;
	int pos_arr = 0;
	/* Start the loop for search */
	for (pos_arr = 0; pos_arr < lenArr - lenSearch; ++pos_arr) {
		if (arr[pos_arr] == toSearch[pos_search]) {
			++pos_search;
			if (pos_search == lenSearch) {	/* Match case */
				return pos_arr;
			}
		} else {
			pos_arr -= pos_search;
			pos_search = 0;
		}	/* Non-match case */
	}
	return -1;
}
