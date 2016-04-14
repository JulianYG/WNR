
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
#define BUF_SIZE 21
#define DELIMITER_LEN 18

static heatshrink_decoder hsd;
static void decompress(uint8_t *input, uint32_t input_size, int device_num);
static int arr_search(uint8_t *toSearch, int lenSearch, uint8_t *arr, int lenArr);
static FILE *csv;

int main(void)
{

    int n,
        cport_nr = 4,        /* 6 (COM5 on windows) */
        bdrate = 38400;       /* 9600 baud */

    uint8_t buf[BUF_SIZE + 1];
    uint8_t data_0[MAX_DATA_SIZE];
    uint8_t data_1[MAX_DATA_SIZE];

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
		    switch(buf[n - 1]) {
		    	case 48:
				    if (arr_search("TKENDTKENDTKENDTKEND", BUF_SIZE - 1, buf, n) >= 0) {
				    	printf("%s\n", "Starting reception on device 0...");
				    	memset(data_0, 0, MAX_DATA_SIZE);	/* Initialize the array */
				    	received_cnt = 0;
				    }

				    else {
					    int pos = -1;
					    /* If receiving the end of current compressed buffer, send & reinitialize */
					    if ((pos = arr_search("TKENDTKENDTKENDTKE", DELIMITER_LEN, buf, n)) >= 0) {	
					    	int eff_len = pos - DELIMITER_LEN + 1;
					    	uint8_t temp[eff_len];
					    	memcpy(temp, buf, eff_len);
					    	memcpy(data_0 + received_cnt, temp, eff_len);
					    	received_cnt += eff_len;
					    	/* Discard the last five bytes of indicators*/
					    	char size_buf[2];
					    	memcpy(size_buf, buf + pos + 1, 2);
					    	uint8_t size = size_buf[1];// + (size_buf[0] << 8);
					    	printf("Received data from device 0 total size: %d\n", size);
					    	
					    	uint8_t comp[size];
					    	memcpy(comp, data_0, size);
					    	decompress(comp, size, 0);

					    	/* Clean up */
					    	memset(data_0, 0, MAX_DATA_SIZE);
					    	received_cnt = 0;	
					    	/* Also need to store rest of the data to avoid loss */
					    	// uint8_t lost[n - eff_len - 5];
					    	// memcpy(lost, buf + pos + 1, n - pos - 1);
					    	// memcpy(data, lost, n - pos - 1);
					    	// received_cnt += n - pos - 1;
					    } else {	/* If regular data packets, store it*/
					    	memcpy(data_0 + received_cnt, buf, n - 1);
					    	received_cnt = received_cnt + n - 1;
					    	/* The extra 1 byte indicating device number */
					    }
					}
					break;
				case 49:
					if (arr_search("TKENDTKENDTKENDTKEND", BUF_SIZE - 1, buf, n) > 0) {
				    	printf("%s\n", "Starting reception on device 1...");
				    	memset(data_1, 0, MAX_DATA_SIZE);	/* Initialize the array */
				    	received_cnt = 0;
				    }

				    else {
					    int pos = -1;
					    /* If receiving the end of current compressed buffer, send & reinitialize */
					    if ((pos = arr_search("TKENDTKENDTKENDTKE", DELIMITER_LEN, buf, n)) >= 0) {	
					    	int eff_len = pos - DELIMITER_LEN + 1;
					    	uint8_t temp[eff_len];
					    	memcpy(temp, buf, eff_len);
					    	memcpy(data_1 + received_cnt, temp, eff_len);
					    	received_cnt += eff_len;
					    	/* Discard the last five bytes of indicators*/
					    	char size_buf[2];
					    	memcpy(size_buf, buf + pos + 1, 2);
					    	int size = size_buf[1] + (size_buf[0] << 4);
					    	printf("Received data from device 1 total size: %d\n", size);

					    	uint8_t comp[size];
					    	memcpy(comp, data_1, size);
					    	decompress(comp, size, 1);

					    	/* Clean up */
					    	memset(data_1, 0, MAX_DATA_SIZE);
					    	received_cnt = 0;	
					    	/* Also need to store rest of the data to avoid loss */
					    	// uint8_t lost[n - eff_len - 5];
					    	// memcpy(lost, buf + pos + 1, n - pos - 1);
					    	// memcpy(data, lost, n - pos - 1);
					    	// received_cnt += n - pos - 1;
					    } else {	/* If regular data packets, store it*/
					    	memcpy(data_1 + received_cnt, buf, n - 1);
					    	received_cnt = received_cnt + n - 1;
					    	/* The extra 1 byte indicating device number */
					    }
					}
					break;
				default:
					break;
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

 void decompress(uint8_t *input, uint32_t compressed_size, int device_num)
 {
 	printf("Decompressing data...\n");
 	heatshrink_decoder_reset(&hsd);
 	size_t decomp_sz = 2048; /* Maximum buffer size before compression */
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
    switch(device_num) {
    	case 0:
		    printf("Size after decompression device 0 data: %d\n", polled);
		    csv = fopen("data_0.csv", "a");
		    for (int i = 0; i < polled; ++i) {
		    	fprintf(csv, "%hhu\n", decomp[i]);
		    }
			fclose(csv);
			break;
		case 1:
			printf("Size after decompression device 1 data: %d\n", polled);
		    csv = fopen("data_1.csv", "a");
		    for (int i = 0; i < polled; ++i) {
		    	fprintf(csv, "%hhu\n", decomp[i]);
		    }
			fclose(csv);
			break;
		default:
			break;
	}
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
