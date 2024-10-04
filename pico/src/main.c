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
    send_buffer = init_buffer(15);

    stdio_init_all();
    tusb_init();
    
    while (true) {
        tud_task(); // USB device task
        if (received_bytes > 0) {
            uint8_t verify_code = verify_received_data(receive_buffer, received_bytes);
            if(verify_code) { // Something went wrong send back an error message
                uint8_t error_message[15];
                snprintf(error_message, sizeof(error_message), "Error code: %d", verify_code);
                send_data(error_message, sizeof(error_message));
                }
            else { // Everything went well send back the ACK
                send_buffer[0] = '\n';
                send_data(send_buffer, 1);
            }
            // reset buffers
            received_bytes = 0;
            clear_buffer(receive_buffer, BUFFER_SIZE);
        } 
       
    }
}

void tud_cdc_rx_cb(uint8_t itf) {
    // This callback is called when data is received
    received_bytes = receive_data(receive_buffer, BUFFER_SIZE);
}