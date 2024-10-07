import serial
from time import time

import include.config as config

def init_serial_connection()->serial:
    PICO_PORT = config.PICO_PORT
    BAUD_RATE = config.BAUD_RATE
    return serial.Serial(PICO_PORT, BAUD_RATE)

def close_serial_connection(serial_port: serial)->None:
    serial_port.close()

def send_data_via_serial_connection(serial_port: serial, data_packet: list)->None:
    # print(f"Sending data: {data_packet}")
    serial_port.write(data_packet) # send data as bytes

def receive_data_via_serial_connection(serial_port: serial)->list:
    if serial_port.in_waiting > 0: # check if there is data waiting to be read
        raw_response = [byte for byte in serial_port.read(serial_port.in_waiting)] # read and decode
        byte_response = bytes(raw_response)
        return byte_response.decode('utf-8')
    else:
        return []

def get_max_data_length_in_block()->int:
    from math import floor
    CDC_MAX_BLOCK_SIZE = 64 # in bytes
    avaliable_data_size_in_block = CDC_MAX_BLOCK_SIZE - 4 # max size - (trailer, blockID, checksum, payload_length)  
    max_leds_in_one_block = floor(avaliable_data_size_in_block/(4 if config.IS_RGBW else 3)) # do not split the led values into different blocks
    max_data_length = max_leds_in_one_block*(4 if config.IS_RGBW else 3) # max data length
    return max_data_length

def compose_data_into_packet(data: list, blockID: int = 0x00):
    message = bytearray()
    message.append(blockID) # blockID [1 byte]
    message.append(len(data)) # payload_length [1 byte]
    message.extend(data) # data [variable length]
    checksum = sum(message[1:len(message)])%256
    message.append(checksum) # checksum [1 byte]
    message.append(0x55) # trailer [1 byte]
    return message

max_data_length = get_max_data_length_in_block()
def data_send_loop(serial_port: serial, screen_data: list):
    index_counter = 0
    blockID = 0x00
    while index_counter<len(screen_data):
        # print(f"Sending blockID: {blockID}")
        try:
           splitted_data = screen_data[index_counter:index_counter+max_data_length]
        except IndexError:
           splitted_data = screen_data[index_counter:] 
        splitted_data_packet = compose_data_into_packet(splitted_data, blockID)
        send_data_via_serial_connection(serial_port, splitted_data_packet)
        # Start the timeout timer
        start_time = time()
        # print(f"Index counter: [{index_counter}:{index_counter+len(splitted_data)}]")

        # Wait for response until the timeout is reached
        while True:
            response = receive_data_via_serial_connection(serial_port)
            # TODO: make intrepret response function
            if response: # if any response is received
                # print(f"Response received: {response}")
                if 'ACK' in response:
                   blockID += 1
                   index_counter += len(splitted_data)
                   break # exit the timeout loop
                elif 'ERROR' in response:
                   return 1 # exit the function with error code
                else:
                    pass # unidentified response, ignore
            elif time() - start_time > config.TIMEOUT_DURATION: # if timeout is reached
                # print(f"No response received within timeout period of {config.TIMEOUT_DURATION} seconds")
                return 2 # exit the function with error code
    return 0 # succesfully sent all data