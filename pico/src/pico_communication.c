#include <stdlib.h>
#include "tusb.h"

#include "pico_communication.h"


void clear_buffer(uint8_t* buffer, int buffer_size) {
    memset(buffer, 0, buffer_size * sizeof(uint8_t));  // Set all bytes to 0
}

uint8_t* init_buffer(int buffer_size) {
    uint8_t* buffer = (uint8_t*) malloc(buffer_size*sizeof(uint8_t));
    if (buffer == NULL) {
        return NULL;
    }
    clear_buffer(buffer, buffer_size);
    return buffer;
}

uint16_t receive_data(uint8_t* receive_buffer, int receive_buffer_size) {
    uint16_t bytes_read = tud_cdc_read(receive_buffer, receive_buffer_size);
    return bytes_read;
}

void send_data(uint8_t* send_buffer, int send_buffer_size) {
   if (send_buffer_size > 0) {
       tud_cdc_write(send_buffer, send_buffer_size); // send the buffer contents
       tud_cdc_write_flush(); // flush to send immediately
       clear_buffer(send_buffer, send_buffer_size);
   }
}
