import include.pc_communication as pc_communication
import time 

# TODO: move this to config

# Port for communication with Pico. Check the correct port from your system (e.g., COM3 on Windows or /dev/ttyUSB0 on Linux/Mac).
PICO_PORT = "/dev/tty.usbmodem212201"
# Baud rate for serial communication with Pico.
BAUD_RATE = 115200
TIMEOUT_DURATION = 2  # Adjust as needed

def main():
    pico_serial = pc_communication.init_serial(PICO_PORT, BAUD_RATE)

    while not pico_serial.is_open: print("Waiting for serial port...")
    
    try:
        while True:
            message = [0x55, 0x0A, 0x01, 0x02, 0x03, 0x04, 0x55]
            print(f"Sending: {message}")
            pc_communication.send_data(pico_serial, message)

            # Start the timeout timer
            start_time = time.time()

            # Wait for response until the timeout is reached
            while time.time() - start_time < TIMEOUT_DURATION:
                response = pc_communication.receive_data(pico_serial)
                if response:
                    print(f"Response: {response}")
                    break  # Break if a response is received
            
            if not response:  # If no response was received
                print("No response received within timeout period.")

            time.sleep(1)  # Wait before sending data again

    except KeyboardInterrupt:
        pass
    finally:
        pc_communication.close_serial(pico_serial)

if __name__ == "__main__":
    main()
