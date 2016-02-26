#include "heatshrink_encoder.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main()
{
	uint8_t data_buf[128]; //{238,239,239,239,240,240,240,240,240,240,240,240,240,239,238,238,238,238,238,238,237,237,236,235,234,233,232,232,231,231,230,230,230,231,231,232,233,233,234,234,234,235,235,235,236,237,238,238,239,240,241,242,243,243,244,244,244,244,244,244,244,243,243,244,244,244,244,244,244,244,244,244,245,245,245,246,246,246,247,247,247,248,248,248,248,248,248,247,247,247,248,248,249,249,249,249,249,249,249,248,248,248,248,248,248,248,248,248,249,249,249,249,248,248,247,247,248,248,248,248,248,248,249,249,249,249,249,248
//};

	for (int i = 0; i < 128; i++) {
		data_buf[i] = i % 15;
	}

	heatshrink_encoder *hse = heatshrink_encoder_alloc(10, 3);

	size_t comp_sz = 128 + (128 / 2) + 4;
	uint8_t *comp = malloc(comp_sz);
	memset(comp, 0, comp_sz);
	size_t count = 0;
	uint32_t sunk = 0;
	uint32_t polled = 0;

	while (sunk < 128) {
        heatshrink_encoder_sink(hse, &data_buf[sunk], 128 - sunk, &count);
        sunk += count;
		if (sunk == 128)
			heatshrink_encoder_finish(hse);
        HSE_poll_res pres;
        do { 
            pres = heatshrink_encoder_poll(hse, &comp[polled], comp_sz - polled, &count);
            polled += count;
        } while (pres == HSER_POLL_MORE);
	}

    for (int i = 0; i < polled; i++) {
		printf("comp %u\n", comp[i]);
    }
  	printf("compressed from %u to %u bytes\n", sunk, polled);

	heatshrink_encoder_free(hse);
}
	