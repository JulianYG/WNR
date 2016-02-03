#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "buffer_mngr.h"

int main(void)
{
	
	uint8_t *a = malloc(sizeof(int) * 250);
	
	for (uint8_t i = 0; i < 120; i ++) {
		a[i] = i * 2;
	}

	// for (uint8_t i = 101; i < 250; i ++) {
	// 	a[i] = 2;
	// }

//	data_buffer db = {};
	buffer_manager bm = {};

	wnr_data sample = {.data = a, .size = 250, .channel_num = 12};

	buffer_manager_init(&bm, 1024, 1, 16);
	buffer_manager_handler(&bm, &sample);

	// buffer_init(&db, 10, sizeof(int));


	// for (int i = 0; i < 10; i++)
	// 	buffer_in(&db, &a[i]);

	// int *r = malloc(sizeof(int));
	// for (int j = 0; j < 5; j++) {
	// 	if (buffer_poll(&db, r) == BUFFER_SUCCESS)
	// 		printf("first round number is %d\n", *r);
	// }

	// for (int i = 3; i < 16; i++)
	// 	 buffer_in(&db, &a[i]);

	// while (buffer_poll(&db, r) == BUFFER_SUCCESS)
	// 	printf("second round number is %d\n", *r);

	// buffer_free(&db);
	// free(r);
	 free(a);
	 buffer_manager_free(&bm);
}

