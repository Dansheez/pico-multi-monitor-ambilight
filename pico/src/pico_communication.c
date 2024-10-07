#include <stdlib.h>
#include "tusb.h"

#include "pico_communication.h"

/// @brief Set all bytes in the buffer to 0. 
/// @param buffer buffer to clear
/// @param buffer_size length of the buffer
void clear_buffer(uint8_t* buffer, int buffer_size) {
    memset(buffer, 0, buffer_size * sizeof(uint8_t));  // Set all bytes to 0
}

/// @brief Create an uint8_t buffer of the specified size. 
/// @param buffer_size length of the buffer to create
/// @return a pointer to the buffer or NULL on failure
uint8_t* init_buffer(int buffer_size) {
    uint8_t* buffer = (uint8_t*) malloc(buffer_size*sizeof(uint8_t));
    if (buffer == NULL) {
        return NULL;
    }
    clear_buffer(buffer, buffer_size);
    return buffer;
}

/// @brief Receive data through the CDC.
/// @param receive_buffer buffer to receive 
/// @param receive_buffer_size length of the buffer 
/// @return number of bytes read 
int pico_receive_data(uint8_t* receive_buffer, int receive_buffer_size) {
    return tud_cdc_read(receive_buffer, receive_buffer_size);
}

/// @brief Send data through the CDC.
/// @param send_buffer buffer to send
/// @param message message to send
void pico_send_data(uint8_t* send_buffer, char message[]) {
    uint8_t message_length = snprintf(send_buffer, SEND_BUFFER_SIZE, "%s", message);
    tud_cdc_write(send_buffer, message_length); // send the buffer contents
    tud_cdc_write_flush(); // flush to send immediately
    clear_buffer(send_buffer, SEND_BUFFER_SIZE);
}

/// @brief Check if the trailer of value 0x55 is on the last received byte 
/// @param receive_buffer buffer to check
/// @param bytes_read length of the buffer
/// @return true if correct
static bool verify_trailer_byte(uint8_t* receive_buffer, int bytes_read) {
    if (receive_buffer[bytes_read-1] != 0x55) {return false;} // check trailer
    return true;
}

/// @brief Check if the blockID byte in the first byte is equal to the blockID passed as parameter
/// @param receive_buffer buffer to check
/// @param blockID expected blockID value
/// @return true if correct
static bool verify_blockID_byte(uint8_t* receive_buffer, uint8_t blockID) {
    if (receive_buffer[0] != blockID) {
        // printf("Block_ID_verification: Expected: %d, Received: %d | %d\n", blockID, receive_buffer[1], blockID!=receive_buffer[1]); 
        return false;}
    return true;
}

/// @brief Check if the payload length byte in the second byte is equal to the number of bytes received. The payload length is the number of bytes in the payload without trailer, blockID, checksum and payload_lenght byte itself.
/// @param receive_buffer buffer to check
/// @param bytes_read length of the buffer
/// @return true if correct
static bool verify_payload_length_byte(uint8_t* receive_buffer, int bytes_read) {
    if (receive_buffer[1]!=bytes_read-4) {
        // printf("Payload_length_verification: Expected: %d, Received: %d | %d\n", receive_buffer[2], bytes_read-5,  bytes_read-5!=receive_buffer[2]);
        return false;}
    return true;
}

/// @brief Check if the checksum on the second to last received byte is correct. The checksum is calculated as the sum of all bytes except the header and trailer modulo 256.
/// @param receive_buffer buffer to check
/// @param bytes_read length of the buffer
/// @return true if correct
static bool verify_checksum_byte(uint8_t* receive_buffer, int bytes_read) {
    uint16_t checksum = 0;
    for (int i=1; i<bytes_read-2; i++) {
        checksum += receive_buffer[i];
    }
    if (checksum%256!=receive_buffer[bytes_read-2]) return false;
    return true;
}

/// @brief Verify the received data. 
/// @param receive_buffer buffer to check
/// @param bytes_read length of the buffer
/// @param block_ID expected blockID value
/// @return 0 if correct, error code otherwise
int verify_received_data(uint8_t* receive_buffer, int bytes_read, uint8_t expected_block_ID) {
    if (!verify_trailer_byte(receive_buffer, bytes_read)) {return 1;}
    if (!verify_blockID_byte(receive_buffer, expected_block_ID)) {return 2;}
    if (!verify_payload_length_byte(receive_buffer, bytes_read)) {return 3;}
    if (!verify_checksum_byte(receive_buffer, bytes_read)) {return 4;}
    return 0;
}
