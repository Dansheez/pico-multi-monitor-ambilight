#include "pico/stdlib.h"

#include "tusb.h"

#include "ws2812.h"
#include "pico_communication.h"
#include "config.h"

// TODO: move this to config
#if IS_RGBW
#define RECEIVE_BUFFER_SIZE LED_N*4+6
uint8_t led_loop_increment = 4; // TODO: don't do that 
#else
#define RECEIVE_BUFFER_SIZE LED_N*3+6
uint8_t led_loop_increment = 3;
#endif
#define SEND_BUFFER_SIZE 15 // TODO: verify if this is enough

uint8_t* receive_buffer;
uint8_t* send_buffer;
uint16_t received_bytes = 0;

#define WS2812_PIN 2
const uint8_t gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

int main()
{
    receive_buffer = init_buffer(RECEIVE_BUFFER_SIZE);
    send_buffer = init_buffer(SEND_BUFFER_SIZE); // mainly for acknowledgement signal and error codes

    stdio_init_all();
    ws2812_init(WS2812_PIN, 800000);
    tusb_init();
    
    while (true) {
        tud_task(); // USB device task
        if (received_bytes > 0) {
            uint8_t verify_code = verify_received_data(receive_buffer, received_bytes);
            if(verify_code) { // Something went wrong send back an error message
                int message_length = snprintf(send_buffer, SEND_BUFFER_SIZE, "Error code: %d", verify_code);
                send_data(send_buffer, message_length);
                }
            else { // Everything went well send back the ACK
                send_buffer[0] = '\n';
                send_data(send_buffer, 1);
                // set leds
                uint32_t led = 0;
                for (int i=3; i<received_bytes; i+=led_loop_increment) {
                    #if IS_RGBW
                    uint32_t led_values = urgbw_u32(gamma8[receive_buffer[i+3]], gamma8[receive_buffer[i+2]], gamma8[receive_buffer[i+1]], gamma8[receive_buffer[i]]);
                    #else
                    uint32_t led_values = urgb_u32(gamma8[receive_buffer[i+2]], gamma8[receive_buffer[i+1]], gamma8[receive_buffer[i]]);
                    #endif
                    ws2812_set_color(led, led_values);
                    led++;
                }
            }
            // reset buffers
            received_bytes = 0;
            clear_buffer(receive_buffer, RECEIVE_BUFFER_SIZE);
            clear_buffer(send_buffer, SEND_BUFFER_SIZE);
        } 
       
    }
}

void tud_cdc_rx_cb(uint8_t itf) {
    // This callback is called when data is received
    received_bytes = receive_data(receive_buffer, RECEIVE_BUFFER_SIZE);
}