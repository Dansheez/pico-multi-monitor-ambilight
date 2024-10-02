#pragma once
#include <stdint.h>

static uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
        return 
            ((uint32_t)r << 16) | 
            ((uint32_t)g << 24) | 
            ((uint32_t)b << 8);
}

static uint32_t urgbw_u32(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
    return 
        ((uint32_t)r << 16) |
        ((uint32_t)g << 24) |
        ((uint32_t)b << 8) |
        ((uint32_t)w << 0);
}

void ws2812_pio_dma_init(int pin, float freq);
void ws2812_set_color(uint32_t led, uint32_t led_values);
void ws2812_dma_update();