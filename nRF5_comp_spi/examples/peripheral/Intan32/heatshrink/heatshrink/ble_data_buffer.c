#include "ble_data_buffer.h"


int ASSERT(COND){
  if (!(COND)) return -1;
}

int ASSERT_EQ(COND1,COND2){
  if (COND1 != COND2) return -1;
}

static void show_error(void)
{   
    LEDS_ON(LEDS_MASK);
    while(1)
    {
        // Do nothing.
    }
}

/** Initialize the buffer by given pointer, maaximum capacity and element size. */
void buffer_init(data_buffer *db, size_t maxCap, size_t sz)
{
    db->buffer = malloc(maxCap * sz);

    if(db->buffer == NULL)
      show_error(); // handle error

    db->buffer_end = (char *)db->buffer + maxCap * sz;

    db->cap = maxCap;
    db->item_cnt = 0;
    db->item_size = sz;
    db->head = db->buffer;

    db->tail = db->buffer;
}


void buffer_reset(data_buffer *db)
{
  db->item_cnt = 0;
  db->head = db->buffer;
  db->tail = db->buffer;
  db->buffer_end = (char *)db->buffer + db->cap * db->item_size;
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
int buffer_compress(uint8_t *db, size_t length, uint8_t *comp)
{
  size_t comp_sz = length + length / 2 + 4;
  comp = malloc(comp_sz);
  memset(comp, 0, comp_sz);
  size_t count = 0;
  
  uint32_t sunk = 0;
  uint32_t polled = 0;
  while (sunk < length) {
      ASSERT(heatshrink_encoder_sink(&hse, &db[sunk], length - sunk, &count) >= 0);
      sunk += count;
        if (sunk == length) {
            ASSERT_EQ(HSER_FINISH_MORE, heatshrink_encoder_finish(&hse));
        }

        HSE_poll_res pres;
        do {                    /* "turn the crank" */
            pres = heatshrink_encoder_poll(&hse, &comp[polled], comp_sz - polled, &count);
            ASSERT(pres >= 0);
            polled += count;
        } while (pres == HSER_POLL_MORE);
        ASSERT_EQ(HSER_POLL_EMPTY, pres);
        if (polled >= comp_sz) {
          printf("compression should never expand that muchr\r\n"); 
          show_error(); 
          return BUFFER_ERROR;
        }
        if (sunk == length) {
            ASSERT_EQ(HSER_FINISH_DONE, heatshrink_encoder_finish(&hse));
        }
    }
//  buffer_reset(db);
  return BUFFER_SUCCESS;
}

