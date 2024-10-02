import mss
from capture_screen import get_screen_capture_config, compose_message_from_screen_data

def main():
    #setup
    get_screen_capture_config()

    # main loop
    with mss.mss() as sct:
        try:
            while True:
                message = compose_message_from_screen_data(sct)
                print(message)
        except KeyboardInterrupt:
            print("Exiting...")
        except Exception as e:
            print(e)
        finally:
            sct.close()

if __name__ == "__main__":
    main()