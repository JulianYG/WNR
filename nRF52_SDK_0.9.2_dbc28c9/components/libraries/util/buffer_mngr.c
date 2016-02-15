#include "buffer_mngr.h"

void buffer_manager_init(buffer_manager* bm, size_t max_data_cap, size_t item_size, int channel_cnt)
{
	bm->c_buf = malloc((max_data_cap + max_data_cap / 16 + 64 + 3) * item_size + 3 * sizeof(size_t) + 3 * sizeof(void *));

	bm->num_channel = channel_cnt;

	for (int i = 0; i < channel_cnt; ++i) {
		buffer_init(&bm->c_buf[i], max_data_cap + max_data_cap / 16 + 64 + 3, sizeof(uint8_t));
	}
}

void buffer_manager_free(buffer_manager* bm)
{
	for (int i = 0; i < bm->num_channel; ++i) {
		buffer_free(&bm->c_buf[i]);
	}
	free(bm->c_buf);
}

int buffer_manager_handler(buffer_manager* bm, wnr_data *item)
{	
	// for (int i = 0; i < item->size; i++) {
	// 	printf("My data before compression is %d\n", item->data[i]);
	// }
	int k = buffer_compress(item->data, item->size, &bm->c_buf[item->channel_num]);
	if (k != BUFFER_SUCCESS) {
		// if not successful, first send out all remaining data
		wnr_data toSend = {.data = bm->c_buf[item->channel_num].buffer, 
			.size = bm->c_buf[item->channel_num].item_cnt, .channel_num = item->channel_num};
	
		buffer_manager_send(&toSend);

		// problem is finding length after compressed data
		// then keep pushing in the rest (pointer arithmetic)
		if (buffer_compress(item->data + k, item->size - k, &bm->c_buf[item->channel_num]) != BUFFER_SUCCESS)
			// this arithmetic not correct

			return -1;
		// just let it be lost if still not successful
	}
		// if buffer full, then reset the buffer

	wnr_data toSend = {.data = bm->c_buf[item->channel_num].buffer, 
		.size = bm->c_buf[item->channel_num].item_cnt, .channel_num = item->channel_num};
	
	buffer_manager_send(&toSend);

	return BUFFER_SUCCESS;
}

int buffer_manager_send(wnr_data *packet)
{
	// for (int i = 0; i < packet->size; i++) {
	// 	printf("My data after compression is: %u\n", (unsigned char) packet->data[i]);
	// }
	printf("My size after compression is: %zu\n, and my channel number is %d.\n", packet->size, packet->channel_num);
	// start polling
	// for (int i = 0; i < 20; ++i) {

	// }
	// if(buffer_poll(&compress_buf, &data2Send[j]) != BUFFER_SUCCESS)
	// 	break;
 //    size_t item_size = bm->c_buf[channel_num]->item_size;
 //    size_t item_cnt = bm->c_buf[channel_num]->item_cnt;
	// buffer_free(bm->c_buf[channel_num]);
	// buffer_init(bm->c_buf[channel_num], item_cnt, item_size);
	return BUFFER_SUCCESS;
}


