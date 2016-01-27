#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "app_error.h"
#include "nrf.h"
#include "bsp.h"
#include "minilzo.h"

#define BUFFER_EMPTY 1
#define BUFFER_SUCCESS 0
#define BUFFER_FULL 2

typedef struct data_buffer
{
	void *buffer;	// data buffer
	void *buffer_end;	// end of data buffer
	size_t cap;	// capacity of the buffer
	size_t item_cnt;	// number of items in the buffer
	size_t item_size;	// size of the item in the buffer
	void *head;	// pointer to the head
	void *tail;	// pointer to the tail
} data_buffer;

void buffer_init(data_buffer *db, size_t maxCap, size_t sz);
void buffer_free(data_buffer *db);
int buffer_in(data_buffer *db, const void *item);
int buffer_poll(data_buffer *db, void *item);
int buffer_compress(data_buffer *db, data_buffer *cb);