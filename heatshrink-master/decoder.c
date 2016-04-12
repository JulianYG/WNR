#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "heatshrink_decoder.h"
#include "heatshrink_common.h"
#include "heatshrink_config.h"
#include "greatest.h"

static heatshrink_decoder hsd;
int main(int argc, char *argv[])
{	
	int data_size = atoi(argv[argc - 1]);
	uint8_t data[data_size];

	for (int i = 0; i < data_size; ++i) {
		data[i] = (uint8_t) argv[1][i];
	}
	
	size_t count = 0;
	size_t decomp_sz = data_size + (data_size / 2) + 4;
	int sunk = 0;
	int polled = 0;
	uint8_t *decomp = malloc(decomp_sz);

	while (sunk < data_size) {
		heatshrink_decoder_sink(&hsd, &data[sunk], data_size - sunk, &count);
		sunk += count;
		HSD_poll_res pres;
		do {
			pres = heatshrink_decoder_poll(&hsd, &decomp[polled],
				decomp_sz - polled, &count);
			printf("%zu\n", count);
			polled += count;
		} while (pres == HSDR_POLL_MORE);
		if (sunk == data_size) {
			HSD_finish_res fres = heatshrink_decoder_finish(&hsd);
		}
	}

	uint8_t buf[polled];
	for (int i = 0; i < polled; ++i) {
		buf[i] = decomp[i];
		printf("%c", buf[i]);
	}
	buf[polled] = '\0';
	printf("%s", buf);
	free(decomp);
	return 0;
}
