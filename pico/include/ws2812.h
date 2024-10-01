#pragma once
#include <stdint.h>

static uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
        return 
            ((uint32_t)r << 8) | 
            ((uint32_t)g <<  16) | 
            ((uint32_t)b <<  0);
}

static uint32_t urgbw_u32(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            ((uint32_t) (w) << 24) |
            (uint32_t) (b);
}

void ws2812_init(int pin, float freq);
void ws2812_update(uint32_t pixel_grb);
