
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
#define BUF_SIZE 1//20
#define DELIMITER_LEN 18
static heatshrink_decoder hsd;
static void decompress(uint8_t *input, uint32_t input_size);
static int arr_search(uint8_t *toSearch, int lenSearch, uint8_t *arr, int lenArr);
static FILE *csv;
//static uint32_t cnt = 0;

int main(void)
{

    int n,
        cport_nr = 6,        /* 4 (COM5 on windows) */
        bdrate = 38400;       /* 9600 baud */

    uint8_t buf[BUF_SIZE + 1];
    uint8_t data[MAX_DATA_SIZE];

    char mode[] = {'8','N','1',0};

    if (RS232_OpenComport(cport_nr, bdrate, mode)) {
        printf("Can not open comport\n");
        return(0);
    }
    int received_cnt = 0;

    while (1) {
      	n = RS232_PollComport(cport_nr, buf, BUF_SIZE);

	    if (n > 0) {
	    	/* always put a "null" at the end of a string! */
		    buf[n] = 0;   
		
		    printf("Received %i bytes: %s\n", n, (char *) buf);

		    if (arr_search("TKENDTKENDTKENDTKEND", BUF_SIZE, buf, n) > 0) {
		    	printf("%s\n", "Starting reception...");
		    	memset(data, 0, MAX_DATA_SIZE);	/* Initialize the array */
		    }

		    else {
	
			    int pos = -1;
			    if ((pos = arr_search("TKENDTKENDTKENDTKE", DELIMITER_LEN, buf, n)) >= 0) {	/* If receiving the end of current compressed buffer, send & reinitialize */
			    	int eff_len = pos - DELIMITER_LEN + 1;
			    	uint8_t temp[eff_len];
			    	memcpy(temp, buf, eff_len);
			    	memcpy(data + received_cnt, temp, eff_len);
			    	received_cnt += eff_len;
			    	/* Discard the last five bytes of indicators*/
			    	char size_buf[2];
			    	memcpy(size_buf, buf + pos + 1, 2);
			    	int size = size_buf[1] + (size_buf[0] << 4);
			    	printf("Received data total size: %d\n", size);

			    	uint8_t comp[size];
			    	memcpy(comp, data, size);


			    	decompress(comp, size);

			    	/* Clean up */
			    	memset(data, 0, MAX_DATA_SIZE);
			    	received_cnt = 0;	
			    	/* Also need to store rest of the data to avoid loss */
			    	// uint8_t lost[n - eff_len - 5];
			    	// memcpy(lost, buf + pos + 1, n - pos - 1);
			    	// memcpy(data, lost, n - pos - 1);
			    	// received_cnt += n - pos - 1;
			    } else {	/* If regular data packets, store it*/
			    	memcpy(data + received_cnt, buf, n);
			    	received_cnt = received_cnt + n;
			    	if (received_cnt == 131) {

			    		decompress(data, 131);
			    	}
			    }
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
 	printf("Decompressing data...\n");
 	heatshrink_decoder_reset(&hsd);
 	size_t decomp_sz = 2048;
	uint8_t *decomp = malloc(decomp_sz);
	memset(decomp, 0, decomp_sz);
	size_t count;
	uint32_t sunk = 0;
    uint32_t polled = 0;

    while (sunk < compressed_size) {
        heatshrink_decoder_sink(&hsd, &input[sunk], compressed_size - sunk, &count);
     
        sunk += count;
        if (sunk == compressed_size) {
            heatshrink_decoder_finish(&hsd);
        }
        HSD_poll_res pres;
        do {
            pres = heatshrink_decoder_poll(&hsd, &decomp[polled],
                decomp_sz - polled, &count);
            polled += count;
        } while (pres == HSDR_POLL_MORE);
        if (sunk == compressed_size) {
            heatshrink_decoder_finish(&hsd);
        }
    }

    csv = fopen("data.csv", "a");
    for (int i = 0; i < polled; ++i) {
    	fprintf(csv, "%hhu\n", decomp[i]);
  //  	cnt += 1;
    //	printf("%hhu", 1, decomp[i]);
    }
	fclose(csv);
	free(decomp);
 }

int arr_search(uint8_t *toSearch, int lenSearch, uint8_t *arr, int lenArr)
{
	int pos_search = 0;
	int pos_arr = 0;
	/* Start the loop for search */
	for (pos_arr = 0; pos_arr < lenArr + lenSearch; ++pos_arr) {
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
