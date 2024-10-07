import mss
from time import sleep, time
import signal
import sys

import include.capture_screen as capture_screen
import include.pc_communication as pc_communication
import include.config as config

def handle_sigterm(signum, frame):
    raise Exception("SIGTERM received. Exiting...") 

# Register the signal handler for SIGTERM
signal.signal(signal.SIGTERM, handle_sigterm)

def main():
    #setup
    capture_screen.get_screen_capture_config()
    pico_serial = pc_communication.init_serial_connection()

    while not pico_serial.is_open: 
        print("Waiting for serial port...")
        sleep(0.5)

    # main loop
    with mss.mss() as sct:
        try:
            while True:
                index_counter = 0
                blockID = 0x00
                screen_data = capture_screen.compose_screen_data(sct)
                print(f"Data send loop exit result: {pc_communication.data_send_loop(pico_serial, screen_data)}")
                sleep(0.5)

        except KeyboardInterrupt:
            print("Exiting...")
        except Exception as e:
            print(f"An error occurred: {e}")
        finally:
            print("Closing...")
            sct.close()
            pc_communication.close_serial_connection(pico_serial)

if __name__ == "__main__":
    main()
