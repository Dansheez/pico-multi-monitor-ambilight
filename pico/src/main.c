#include "pico/stdlib.h"

#include "tusb.h"

#include "ws2812.h"
#include "pico_communication.h"
#include "config.h"

#define SEND_BUFFER_SIZE 15 // TODO: verify if this is enough
#define RECEIVE_BUFFER_SIZE 64
#define WS2812_PIN 2

uint8_t* receive_buffer;
uint8_t* send_buffer;
int bytes_read = 0;

/// @brief Get a message from the CDC, verify it and assign its content to the LEDs buffer 
/// @return 0 if correct, error code otherwise
int get_message_and_assign_leds() {
    uint8_t expected_blockID = 0x00;
    uint16_t led_count = 0;
    while (true) {
        tud_task(); // USB device task
        if (bytes_read>0) {
            int error = verify_received_data(receive_buffer, bytes_read, expected_blockID);
            char message[15];
            sprintf(message, "ERROR: %d\n", error);
            if (error) {pico_send_data(send_buffer, message); return 1;}
            led_count = ws2812_set_color_from_buffer(led_count, receive_buffer, bytes_read);
            clear_buffer(receive_buffer, RECEIVE_BUFFER_SIZE);
            bytes_read = 0;
            pico_send_data(send_buffer, "ACK"); // ACK
            if (led_count >= LED_N) {return 0;}
            expected_blockID++;
            }
        }
    }

int main()
{
    receive_buffer = init_buffer(RECEIVE_BUFFER_SIZE);
    send_buffer = init_buffer(SEND_BUFFER_SIZE); // mainly for acknowledgement signal and error codes

    stdio_init_all();
    ws2812_init(WS2812_PIN, 800000);
    tusb_init();
    
    while (true) {
        get_message_and_assign_leds();
        bytes_read = 0;
    }
}

void tud_cdc_rx_cb(uint8_t itf) {
    // This callback is called when data is received   
    bytes_read = pico_receive_data(receive_buffer, RECEIVE_BUFFER_SIZE);
}