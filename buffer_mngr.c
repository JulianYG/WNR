#include "buffer_mngr.h"

void buffer_manager_init(buffer_manager* bm, size_t max_data_cap, size_t item_size, int channel_cnt)
{
	bm->c_buf = malloc((max_data_cap + max_data_cap / 16 + 64 + 3) * item_size);

	bm->num_channel = channel_cnt;

	for (int i = 0; i < channel_cnt; ++i) {
		buffer_init(*bm->c_buf[i], max_data_cap + max_data_cap / 16 + 64 + 3);
	}
}

void buffer_manager_free(buffer_manager* bm)
{
	for (int i = 0; i < bm->num_channel; ++i) {
		buffer_free(*bm->c_buf[i]);
	}
	free(bm->c_buf);
}

int buffer_manager_handler(buffer_manager* bm, wnr_data *item)
{
	if ((int k = buffer_compress(item->data, item->size, bm->c_buf[item->channel_num])) != BUFFER_SUCCESS) {
		// if not successful, first send out all remaining data
		wnr_data toSend = {.data = bm->c_buf[item->channel_num], 
			.size = sizeof(*(bm->c_buf[item->channel_num])), .channel_num = item->channel_num};
	
		buffer_manager_send(&toSend);

		// then keep pushing in the rest (pointer arithmetic)
		buffer_compress(item->data + k, item->size - k, bm->c_buf[item->channel_num]);
		// just let it be lost if still not successful
	}
     
		return -1;
		// if buffer full, then reset the buffer

	wnr_data toSend = {.data = bm->c_buf[item->channel_num], 
		.size = sizeof(*(bm->c_buf[item->channel_num])), .channel_num = item->channel_num};
	
	buffer_manager_send(&toSend);

	return BUFFER_SUCCESS;
}

int buffer_manager_send(wnr_data *packet)
{
	// for (int i = 0; i < 20; ++i) {

	// }
	// if(buffer_poll(&compress_buf, &data2Send[j]) != BUFFER_SUCCESS)
	// 	break;
 //    size_t item_size = bm->c_buf[channel_num]->item_size;
 //    size_t item_cnt = bm->c_buf[channel_num]->item_cnt;
	// buffer_free(bm->c_buf[channel_num]);
	// buffer_init(bm->c_buf[channel_num], item_cnt, item_size);
}


