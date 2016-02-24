#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "buffer_mngr.h"

int main(void)
{
	int *a = malloc(sizeof(int) * 10);
	for (int i = 0; i < 10; i ++) {
		a[i] = i + 1;
	}
	data_buffer *buf;
	buffer_init(buf, 10, sizeof(int));
	for (int i = 0; i < 5; i++)
		buffer_in(buf, &a[i]);

	int *r = malloc(sizeof(int));
	while (buffer_poll(buf, r) == BUFFER_SUCCESS)
		printf("number is %d", *r);

}