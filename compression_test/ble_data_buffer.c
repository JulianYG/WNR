#include "ble_data_buffer.h"

/* Work-memory needed for compression. Allocate memory in units
 * of 'lzo_align_t' (instead of 'char') to make sure it is properly aligned.
 */

#define HEAP_ALLOC(var,size) \
    lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]

static HEAP_ALLOC(wrkmem, LZO1X_1_MEM_COMPRESS);
// Assign working memory for compression alg

// static void show_error(void)
// {   
//     LEDS_ON(LEDS_MASK);
//     while(1)
//     {
//         // Do nothing.
//     }
// }

/** Initialize the buffer by given pointer, maaximum capacity and element size. */
void buffer_init(data_buffer *db, size_t maxCap, size_t sz)
{
	  db->buffer = malloc(maxCap * sz);

  	// if(db->buffer == NULL)
   //  	show_error();	// handle error
  	

    db->buffer_end = (char *)db->buffer + maxCap * sz;


  	db->cap = maxCap;
  	db->item_cnt = 0;
  	db->item_size = sz;
  	db->head = db->buffer;

  	db->tail = db->buffer;
}

/** Empty and reset the buffer. */
void buffer_free(data_buffer *db)
{
  free(db->buffer);
}

/** Insert data into the buffer.*/ // is it really cyclic?
int buffer_in(data_buffer *db, const void *item)
{ 
    if(db->item_cnt == db->cap) {       
        return BUFFER_FULL;// handle error
  }

    memcpy(db->head, item, db->item_size);
    db->head = (char*)db->head + db->item_size;
    if(db->head == db->buffer_end)
        db->head = db->buffer;
    db->item_cnt++;
   return BUFFER_SUCCESS;
}

/** Pop out data from buffer. */
int buffer_poll(data_buffer *db, void *item)
{
    if(db->item_cnt == 0)
        return BUFFER_EMPTY;// handle error
    memcpy(item, db->tail, db->item_size);
    db->tail = (char*)db->tail + db->item_size;
    if(db->tail == db->buffer_end)
        db->tail = db->buffer;
    db->item_cnt--;
    return BUFFER_SUCCESS;
}

/** The core of compression. */
int buffer_compress(uint8_t *db, size_t length, data_buffer *cb)
{
  lzo_uint in_len = (lzo_uint) sizeof(uint8_t) * length;
  // the input length is the total size of data buffer
  lzo_uint out_len = in_len + in_len / 16 + 64 + 3;
  // preparing extra space for output compression buffer
  // unsigned char is also 8 bits
  unsigned char __LZO_MMODEL out[out_len];
  // initialize pointers for compression input/output
  // for (int j = 0; j < db->item_cnt; ++j) {
  //  if(buffer_poll(db, &in[j]) != BUFFER_SUCCESS)
  //    // feed data into compression input until exhausted
  //    break;
  // }
  lzo1x_1_compress(db, in_len, out, &out_len, wrkmem);
  // push the compressed data into compression buffer
  for (int k = 0; k < out_len; ++k) {
    if (buffer_in(cb, &out[k]) != BUFFER_SUCCESS)
      // when buffer full
      return k;
  }
  return BUFFER_SUCCESS;
}