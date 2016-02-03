#include "ble_data_buffer.h"

typedef struct buffer_manager
{
	data_buffer* d_buf;
	data_buffer* c_buf;
	int num_channel;
}	buffer_manager;

void buffer_manager_init(buffer_manager* bm, size_t max_data_cap, size_t item_size, int channel_cnt);
void buffer_manager_free(buffer_manager* bm);
int buffer_manager_handler(buffer_manager* bm, wnr_data *item);
int buffer_manager_send(wnr_data *packet);
