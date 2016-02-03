#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "buffer_mngr.h"

int main(void)
{
	
	int *a = malloc(sizeof(int) * 20);
	
	for (int i = 0; i < 20; i ++) {
		a[i] = i + 1;
	}

//	data_buffer db = {};
	buffer_manager bm = {};

	wnr_data sample = {};

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
}

