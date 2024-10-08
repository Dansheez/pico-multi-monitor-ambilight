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
    """How many data bytes can be sent in one block. 
    CDC_MAX_BLOCK_SIZE - (trailer, blockID, checksum, payload_length)
    """
    from math import floor
    CDC_MAX_BLOCK_SIZE = 64 # in bytes
    avaliable_data_size_in_block = CDC_MAX_BLOCK_SIZE - 4 # max size - (trailer, blockID, checksum, payload_length)  
    max_leds_in_one_block = floor(avaliable_data_size_in_block/(4 if config.IS_RGBW else 3)) # do not split the led values into different blocks
    max_data_length = max_leds_in_one_block*(4 if config.IS_RGBW else 3) # max data length
    return max_data_length

def create_data_block(data: list, blockID: int = 0x00)->bytearray:
    """Block consists of:
    - blockID [1 byte]
    - payload_length [1 byte] (only length of data, not including blockID, checksum and trailer)
    - data [variable length]
    - checksum [1 byte] (sum of all previous bytes modulo 256)
    - trailer [1 byte]

    Args:
        data (list): data to be packed into a block
        blockID (int, optional): ID of the block. Defaults to 0x00.

    Returns:
        bytearray: message block ready to be sent
    """
    message = bytearray()
    message.append(blockID) # blockID [1 byte]
    message.append(len(data)) # payload_length [1 byte]
    message.extend(data) # data [variable length]
    checksum = sum(message[1:len(message)])%256
    message.append(checksum) # checksum [1 byte]
    message.append(0x55) # trailer [1 byte]
    return message

def wait_for_response_and_interpret(serial_port: serial)->int:
    """
    Returns:
        int:  0 - success, 1 - error, 2 - timeout 
    """
    start_time = time()
    while True:
        response = receive_data_via_serial_connection(serial_port)
        if response:
            # print(f"Response received: {response}")
            if 'ACK' in response: return 0
            elif 'ERROR' in response: return 1
        elif time() - start_time > config.TIMEOUT_DURATION: # if timeout is reached
            return 2

max_data_length = get_max_data_length_in_block()
def data_send_loop(serial_port: serial, screen_data: list)->int:
    """Sends the whole packet of data in one or multiple blocks if it is too large. 
    Waits for the response after each block until the timeout is reached.

    Args:
        serial_port (serial): serial port to send the data
        screen_data (list): raw data to be split, packed into blocks and sent

    Returns:
        int:  0 - success, 1 - error, 2 - timeout
    """
    index_counter = 0
    blockID = 0x00
    while index_counter<len(screen_data):
        try:
           splitted_data = screen_data[index_counter:index_counter+max_data_length]
        except IndexError:
           splitted_data = screen_data[index_counter:] 
        splitted_data_packet = create_data_block(splitted_data, blockID)
        send_data_via_serial_connection(serial_port, splitted_data_packet)
        if(ret:=wait_for_response_and_interpret(serial_port)):
            return ret # error code
        blockID += 1
        index_counter += len(splitted_data)
    return 0 # succesfully sent all data