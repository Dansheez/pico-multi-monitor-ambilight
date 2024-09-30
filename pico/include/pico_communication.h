#pragma once 

void clear_buffer(uint8_t* buffer, int buffer_size);
uint8_t* init_buffer(int buffer_size);
uint16_t receive_data(uint8_t* receive_buffer, int receive_buffer_size);
void send_data(uint8_t* send_buffer, int send_buffer_size);