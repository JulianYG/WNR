#include "ble_data_buffer.h"

typedef struct buffer_manager
{
	data_buffer* d_buf;
	data_buffer* c_buf;
	int num_channel;
}	buffer_manager;

void buffer_manager_init(buffer_manager* bm, size_t max_data_cap, size_t item_size, int channel_cnt);
void buffer_manager_free(buffer_manager* bm);
int buffer_manager_store(buffer_manager* bm, int channel_num, const void *item, void *flush_array);
int buffer_manager_send(buffer_manager* bm, int channel_num, void *send_array);
