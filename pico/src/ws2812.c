#include <stdio.h>

#include "ws2812.h"
#include "ws2812.pio.h"
#include "config.h"

#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/clocks.h"

#define LED_BUFFER_SIZE LED_N
// only visible in this file
static uint32_t led_buffer[LED_BUFFER_SIZE];
static int dma_chan;

static void ws2812_program_init(PIO pio, uint sm, uint offset, uint pin, float freq, bool rgbw) {
    /*
    Arguments:
    pio - Which of the PIO instances we are dealing with
    sm - Which state machine on that PIO we want to configure to run the WS2812B program
    offset - Where the PIO program was loaded in PIO's 5-bit program address space
    pin - Which GPIO pin our WS2812 LED chain is conected to
    freq - The frequency (or rather baud rate) we want to output data at
    rgbw - True if we are using 4-colour LEDs (red, green, blue, white) rather than usual 3.
    */
        
    pio_gpio_init(pio, pin);    // Configure a GPIO for use by PIO (Set the GPIO function select)
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);   // Sets the PIO pin direction of 1 pin starting at pin number "pin" to out  
        
    pio_sm_config c = ws2812_program_get_default_config(offset);   // Get the default configuration using the generated function for this program (this includes things like .warp and .side_set configurations from the program). We'll modify this configuration before loading it into the state machie.
    sm_config_set_sideset_pins(&c, pin);    // Sets the side-set to write to pins starting at pin "pin" (we say starting at because if you had .side_set 3, then it would be outputting values on numbers "pin", "pin+1", "pin+2")
    sm_config_set_out_shift(&c, false, true, rgbw ? 32:24);     // False for shift_to_right (i.e. we want to shift out MostSignificantBit first). True for autopull. 32 or 24 for the number of bits for the autopull threshold, i.e. the point at which the state machine triggers a refill of the OSR, depending on whether the LEDs are RGB or RGBW.
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);

    /*
    Step-by-step calculation of time per cycle:
    1. Determine "cycles_per_bit": 
        T1+T2+T3 = 3+3+4 = 10 cycles.
    2. Calculate the PIO clock frequency "pio_clk": 
        div = clk_sys_in_Hz(i.e. 125 MHz) / freq (i.e. 800kHz) * cycles_per_bit 
        div = 125_000_000 / 8_000_000 =  15.625
    3. Calculate the time for one cycle: 
        time_per_cycle = div / clk_sys_in_Hz
        in this case: time_per_cycle = 15.625 / 125_000_000 = 0.125us = 125ns
    */
    int cycles_per_bit = ws2812_T1 + ws2812_T2 + ws2812_T3;  // This is the total number of execution cycles to output a single bit. Here we see the benefit of .define public; we can use the T1 - T3 values in our code.
    float div = clock_get_hz(clk_sys) / (freq * cycles_per_bit);    // Slow the state mahcine's execution down, based on the system clock speed and the number of execution cycles required per WS2812B data bit, so that we achieve the correct bit rate.
    sm_config_set_clkdiv(&c, div);

    pio_sm_init(pio, sm, offset, &c);   // Load our configuration into the state machine, and go to the start address ("offset")
    pio_sm_set_enabled(pio, sm, true);  // And make it go now!
}

void ws2812_pio_dma_init(int pin, float freq) {
    // PIO machine init
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, pin, freq, IS_RGBW);
    
    // DMA init
    dma_chan = dma_claim_unused_channel(true);
    dma_channel_config c = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
    channel_config_set_read_increment(&c, true);
    channel_config_set_dreq(&c, pio_get_dreq(pio, sm, true));
    
    dma_channel_configure(
        dma_chan,
        &c,
        &pio->txf[sm],
        led_buffer,
        LED_BUFFER_SIZE,
        false
    );
    // LED buffer init
    for (int i = 0; i < LED_BUFFER_SIZE; i++) {
        led_buffer[i] = 0x00;
    }
}

void ws2812_set_color(uint32_t led, uint32_t led_values) {
    if (led < LED_N) {
        led_buffer[led] = led_values;
    }
}

void ws2812_dma_update() {
    dma_channel_start(dma_chan);
    dma_channel_wait_for_finish_blocking(dma_chan);
    dma_channel_set_read_addr(dma_chan, led_buffer, false);
}