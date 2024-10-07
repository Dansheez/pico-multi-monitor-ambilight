import mss
from time import sleep, time
import signal
import sys
import asyncio
import time

import include.capture_screen as capture_screen
import include.pc_communication as pc_communication
import include.config as config

def handle_sigterm(signum, frame):
    raise Exception("SIGTERM received. Exiting...") 

# Register the signal handler for SIGTERM
signal.signal(signal.SIGTERM, handle_sigterm)


async def capture_screen_job(sct: mss, delay: float, pico_serial):
    while True:
        start_time = time.time()
        message = capture_screen.compose_screen_data(sct)
        pc_communication.send_data(pico_serial, message)

        # Wait for response until the timeout is reached
        while True:
            response = pc_communication.receive_data(pico_serial) 
            if response:
                print(f"Response: {response}")
                break  # Break if a response is received
            elif (time.time() - start_time) >= delay:
                print("No response received within timeout period.")
                break
        await asyncio.sleep(max(0, delay - (time.time() - start_time)))

async def main():
    #setup
    capture_screen.get_screen_capture_config()
    pico_serial = pc_communication.init_serial()

    while not pico_serial.is_open: 
        print("Waiting for serial port...")
        sleep(0.5)

    # main loop
    try:
        with mss.mss() as sct:
            delay = 1/config.TARGET_FPS
            await capture_screen_job(sct, delay, pico_serial) 
    except KeyboardInterrupt:
        print("Exiting...")
    except Exception as e:
        print(f"Error: {e}")
    finally:
        sct.close()

if __name__ == "__main__":
    asyncio.run(main())