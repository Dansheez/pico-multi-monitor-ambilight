import mss
import asyncio
import time

import include.capture_screen as capture_screen
import include.config as config


async def capture_screen_job(sct: mss, delay: float):
    while True:
        start_time = time.time()
        message = capture_screen.compose_message_from_screen_data(sct)
        print(f"{message}")
        await asyncio.sleep(max(0, delay - (time.time() - start_time)))

async def main():
    capture_screen.get_screen_capture_config()
    # main loop
    try:
        with mss.mss() as sct:
            delay = 1/config.TARGET_FPS
            await capture_screen_job(sct, delay) 
    except KeyboardInterrupt:
        print("Exiting...")
    except Exception as e:
        print(f"Error: {e}")
    finally:
        sct.close()

if __name__ == "__main__":
    asyncio.run(main())