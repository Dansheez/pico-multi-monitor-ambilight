import mss
from time import sleep, time

import include.capture_screen as capture_screen
import include.pc_communication as pc_communication
import include.config as config

def main():
    #setup
    capture_screen.get_screen_capture_config()
    pico_serial = pc_communication.init_serial()

    while not pico_serial.is_open: 
        print("Waiting for serial port...")
        sleep(0.5)

    # main loop
    with mss.mss() as sct:
        try:
            while True:
                message = capture_screen.compose_message_from_screen_data(sct)
                print(f"Sending: {message}")
                response = pc_communication.receive_data(pico_serial)
                # Start the timeout timer
                start_time = time()

                # Wait for response until the timeout is reached
                while True:
                    response = pc_communication.receive_data(pico_serial) 
                    if response:
                        print(f"Response: {response}")
                        break  # Break if a response is received
                    if (time() - start_time) >= config.TIMEOUT_PERIOD:
                        print("No response received within timeout period.")
                        break

                sleep(0.5)

        except KeyboardInterrupt:
            print("Exiting...")
        except Exception as e:
            print(f"An error occurred: {e}")
        finally:
            print("Closing...")
            sct.close()
            pc_communication.close_serial(pico_serial)

if __name__ == "__main__":
    main()
