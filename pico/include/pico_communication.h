#pragma once 
#include "config.h"

#define RECEIVE_BUFFER_SIZE 64
#define SEND_BUFFER_SIZE 15

void clear_buffer(uint8_t* buffer, int buffer_size);
uint8_t* init_buffer(int buffer_size);
int pico_receive_data(uint8_t* receive_buffer, int receive_buffer_size);
void pico_send_data(uint8_t* send_buffer, char message[]);
void pico_get_message(uint8_t* receive_buffer, uint8_t* send_buffer);
int verify_received_data(uint8_t* receive_buffer, int bytes_read, uint8_t expected_block_ID);