#include "pico/stdlib.h"
#include <stdint.h>
#include <stdlib.h>

#include "ws2812.h"
#include "config.h"

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

bool repeating_timer_callback(__unused struct repeating_timer *t){
    ws2812_dma_update();
    return true;
}

int main()
{
    stdio_init_all();
    ws2812_pio_dma_init(WS2812_PIN, 800000); // 800 kHz freq for WS2812

    double* delay_s = (double*)malloc(sizeof(double));
    if (delay_s != NULL) *delay_s = 1/(double)LED_FREQ;

    struct repeating_timer timer;
    add_repeating_timer_us(
        -*delay_s * 1000000,
        repeating_timer_callback,
        NULL,
        &timer
    );
    free(delay_s);

    while (true) {
        for (int led=0; led<LED_N; led++) {
            uint8_t r = gamma8[rand() % 256];
            uint8_t g = gamma8[rand() % 256];
            uint8_t b = gamma8[rand() % 256];
            #if IS_RGBW
            uint8_t w = gamma8[rand() % 256];
            ws2812_set_color(led, urgbw_u32(r, g, b, w));
            #else
            ws2812_set_color(led, urgb_u32(r, g, b));
            #endif
        }
        sleep_ms(100);
    }
}
