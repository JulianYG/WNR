#include "heatshrink_encoder.h"

int main()
{
	uint8_t data_buf = {2,3,4,5,67,8,9,0,1,2,3,4,2,5,4,3,3,2};

	heatshrink_encoder *hse = heatshrink_encoder_alloc(10, 3);
			size_t comp_sz = 64 + (64 / 2) + 4;
			uint8_t *comp = malloc(comp_sz);
			memset(comp, 0, comp_sz);
			size_t count = 0;
			uint32_t sunk = 0;
			uint32_t polled = 0;
			
			while (sunk < 64) {
        heatshrink_encoder_sink(hse, data_buf[sunk], 64 - sunk, &count);
        sunk += count;
				printf("sunk %lu", sunk);
				if (sunk == 64)
				heatshrink_encoder_finish(hse);
				
        HSE_poll_res pres;
        do {                    /* "turn the crank" */
            pres = heatshrink_encoder_poll(hse, &comp[polled], comp_sz - polled, &count);
            polled += count;
        } while (pres == HSER_POLL_MORE);
			}
	  		printf("%zu", polled);
      for (int i = 0; i < polled; i++) {
			printf("%comp uhh", comp[i]);

      }
			

			heatshrink_encoder_free(hse);
}
	