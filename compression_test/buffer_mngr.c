#include "buffer_mngr.h"

void buffer_manager_init(buffer_manager* bm, size_t max_data_cap, size_t item_size, int channel_cnt)
{
	bm = malloc((2 * max_data_cap + max_data_cap / 16 + 64 + 3) * item_size * channel_cnt);

	bm->d_buf = malloc(max_data_cap * item_size);
	bm->c_buf = malloc((max_data_cap + max_data_cap / 16 + 64 + 3) * item_size);

	bm->num_channel = channel_cnt;

	for (int i = 0; i < channel_cnt; ++i) {
		buffer_init(*bm->d_buf[i], max_data_cap, item_size);
		buffer_init(*bm->c_buf[i], max_data_cap + max_data_cap / 16 + 64 + 3);
	}
}

void buffer_manager_free(buffer_manager* bm)
{
	for (int i = 0; i < bm->num_channel; ++i) {
		buffer_free(*bm->d_buf[i]);
		buffer_free(*bm->c_buf[i]);
	}
	free(bm->d_buf);
	free(bm->c_buf);
	free(bm);
}

int buffer_manager_store(buffer_manager* bm, int channel_num, const void *item, void *flush_array)
{
	if (buffer_in(bm->d_buf[channel_num], item) == BUFFER_FULL) {
        /* 
         * If buffer full, compress it and reset. Assuming compression is deep copy..
         */
		if (buffer_compress(bm->d_buf[channel_num], bm->c_buf[channel_num]) != BUFFER_SUCESS) {
			
			buffer_manager_send(bm, channel_num, flush_array);
			buffer_compress(bm->d_buf[channel_num], bm->c_buf[channel_num]);
		}

	//	size_t item_size = bm->d_buf[channel_num]->item_size;
	//	size_t item_cnt = bm->d_buf[channel_num]->item_cnt;

	//	buffer_free(bm->d_buf[channel_num]);
	//	buffer_init(bm->d_buf[channel_num], item_cnt, item_size);

		// the compressing process pops out every element in data buffer and make it empty
		buffer_in(bm->d_buf[channel_num], item);
		return channel_num;
	}	// if buffer full, then reset the buffer
	return STORE_SUCCESS;
}

int buffer_manager_send(buffer_manager* bm, int channel_num, void *send_array)
{
	for (int i = 0; i < 20; ++i) {

	}
	if(buffer_poll(&compress_buf, &data2Send[j]) != BUFFER_SUCCESS)
		break;
    size_t item_size = bm->c_buf[channel_num]->item_size;
    size_t item_cnt = bm->c_buf[channel_num]->item_cnt;
	buffer_free(bm->c_buf[channel_num]);
	buffer_init(bm->c_buf[channel_num], item_cnt, item_size);
}


