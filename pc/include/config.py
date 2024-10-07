# **MONITOR CONFIGURATION**
# Number of monitors in use.
MONITOR_COUNT = 1

# Width of an area to calculate dominant color (in pixels).
AREA_WIDTH = 100

# Target frequency of capturing the screen (in Hz).
TARGET_FPS = 30

# **LED CONFIGURATION**
# Specified order in which the LED strips are connected to each side (left, bottom, right, top) for each monitor.
# 0 - first
# None - not used
LED_CONNECTION_LAYOUT = [
    {"left": 0, "bottom": 1, "right": 2, "top": 3}, # monitor 1
    # additional monitors here
] 

# Total number of LEDs around each monitor breakdown by each side
# (MATCH WITH LED_LAYOUT ORDER)
# TODO: map_led_sides to work undependently from LED_LAYOUT 
LED_COUNT = [
    {"left": 1, "bottom": 2, "right": 1, "top": 2}, # monitor 1
    # additional monitors here
]

# **COMMMUNICATION CONFIGURATION**
# Serial port for communication with Pico. Check the correct port from your system (e.g., COM3 on Windows or /dev/ttyUSB0 on Linux/Mac).
PICO_PORT = "/dev/tty.usbmodem212401"

# Baud rate for serial communication with Pico.
BAUD_RATE = 115200

# Timeout duration for serial communication with Pico.
TIMEOUT_DURATION = 2  # seconds