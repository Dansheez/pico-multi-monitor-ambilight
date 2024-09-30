#include "pico/stdlib.h"

#include "tusb.h"
#include "pico_communication.h"

// TODO: move this to config
#define BUFFER_SIZE 64
uint8_t* receive_buffer;
uint8_t* send_buffer;
uint16_t received_bytes = 0;

int main()
{
    receive_buffer = init_buffer(BUFFER_SIZE);
    send_buffer = init_buffer(BUFFER_SIZE);

    stdio_init_all();
    tusb_init();
    
    while (true) {
        tud_task(); // USB device task
        if (received_bytes > 0) {
            send_buffer = receive_buffer;
            send_data(send_buffer, received_bytes);
            received_bytes = 0;
        } 
       
    }
}

void tud_cdc_rx_cb(uint8_t itf) {
    // This callback is called when data is received
    received_bytes = receive_data(receive_buffer, BUFFER_SIZE);
}