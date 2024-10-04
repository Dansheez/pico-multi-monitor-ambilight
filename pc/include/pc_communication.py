import serial

import include.config as config

def init_serial():
    PICO_PORT = config.PICO_PORT
    BAUD_RATE = config.BAUD_RATE
    return serial.Serial(PICO_PORT, BAUD_RATE)

def close_serial(serial_port):
    serial_port.close()

def send_data(serial_port: serial, data: list)->None:
    # TODO: add blockID support
    print(f"Sending data: {data}")
    packed_data = compose_data_into_packet(data)
    serial_port.write(packed_data) # send data as bytes

def receive_data(serial_port: serial)->list:
    if serial_port.in_waiting > 0: # check if there is data waiting to be read
        raw_response = [byte for byte in serial_port.read(serial_port.in_waiting)] # read and decode
        byte_response = bytes(raw_response)
        return byte_response.decode('utf-8')
    else:
        return []

def compose_data_into_packet(data: list, blockID: int = 0x00):
    message = bytearray()
    message.append(0x55) # header [1 byte]
    message.append(blockID) # blockID [1 byte]
    message.append(len(data)%256) # payload_length [1 byte] FIXME: remove modulo after blockID support
    message.extend(data) # data [variable length]
    checksum = sum(message[1:len(message)])%256
    message.append(checksum) # checksum [1 byte]
    message.append(0x55) # trailer [1 byte]
    return message