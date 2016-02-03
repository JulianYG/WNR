#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "buffer_mngr.h"

int main(int argc, char *argv[])
{
	FILE *fp = fopen(argv[1], "r");
	fseek(fp, 0, SEEK_END);
	
    long file_len = ftell(fp);
    rewind(fp);


    uint8_t *buffer = (uint8_t *) malloc((file_len + 1) * sizeof(uint8_t));
    fread(buffer, file_len, 1, fp);

  
    fclose(fp);

	// uint8_t *a = malloc(sizeof(int) * 50);
	
	// for (uint8_t i = 0; i < 25; i ++) {
	// 	a[i] = 0;
	// }

	// for (uint8_t i = 25; i < 50; i ++) {
	// 	a[i] = 1;
	// }

	//	data_buffer db = {};
	buffer_manager bm = {};

	wnr_data sample = {.data = buffer, .size = file_len, .channel_num = 12};

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
	// free(a);
	free(buffer);
	buffer_manager_free(&bm);
}

