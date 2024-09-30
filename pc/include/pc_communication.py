import serial

def init_serial(serial_port, baud_rate):
    return serial.Serial(serial_port, baud_rate)

def close_serial(serial_port):
    serial_port.close()

def send_data(serial_port: serial, data: list)->None:
    byte_data = bytes(data)
    serial_port.write(byte_data) # send data as bytes

def receive_data(serial_port: serial)->list:
    if serial_port.in_waiting > 0: # check if there is data waiting to be read
        return [byte for byte in serial_port.read(serial_port.in_waiting)] # read and decode
    else:
        return []