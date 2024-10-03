import mss
import include.capture_screen as capture_screen

def main():
    #setup
    capture_screen.get_screen_capture_config()

    # main loop
    with mss.mss() as sct:
        try:
            while True:
                message = capture_screen.compose_message_from_screen_data(sct)
                print(message)
        except KeyboardInterrupt:
            print("Exiting...")
        except Exception as e:
            print(e)
        finally:
            sct.close()

if __name__ == "__main__":
    main()