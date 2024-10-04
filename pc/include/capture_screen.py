import mss
import numpy as np
from fast_colorthief import get_dominant_color

import include.config as config

def get_screen_capture_config()->None:
    global MONITOR_COUNT, AREA_WIDTH, LED_CONNECTION_LAYOUT, LED_COUNT, LED_MAP

    MONITOR_COUNT = config.MONITOR_COUNT
    AREA_WIDTH = config.AREA_WIDTH
    LED_CONNECTION_LAYOUT = config.LED_CONNECTION_LAYOUT
    LED_COUNT = config.LED_COUNT
    LED_MAP = map_led_sides()

def map_led_sides():
    led_map = []
    # Loop through monitors
    for monitor_index, (connection_layout, led_count) in enumerate(zip(LED_CONNECTION_LAYOUT, LED_COUNT)):
        # Check each side of the monitor
        for side in ["left", "bottom", "right", "top"]:
            if connection_layout[side] is not None:  # Check if this side has LEDs
                led_map.append({
                    "monitor": monitor_index + 1,  # 1-based monitor numbering
                    "side": side,
                    "led_count": led_count[side],
                    "layout_index": connection_layout[side]
                })
    
    # Sort by the layout index to maintain the correct order
    led_map.sort(key=lambda x: x["layout_index"])
    
    return led_map

# TODO: map the regions as constants for each monitor in config init, instead of computing it every time here (only assign here)
def identify_border_area(side: str, monitor: dict)->dict[int, int, int, int]:
    if side == "top":
        region = {
            "left": monitor["left"],
            "top": monitor["top"],
            "width": monitor["width"],
            "height": AREA_WIDTH
        }
    elif side == "bottom":
        region = {
            "left": monitor["left"],
            "top": monitor["top"]+monitor["height"]-AREA_WIDTH,
            "width": monitor["width"],
            "height": AREA_WIDTH
        }
    elif side == "left":
        region = {
            "left": monitor["left"],
            "top": monitor["top"],
            "width": AREA_WIDTH,
            "height": monitor["height"] 
        }
    elif side == "right":
        region = {
            "left": monitor["left"]+monitor["width"]-AREA_WIDTH,
            "top": monitor["top"],
            "width": AREA_WIDTH,
            "height": monitor["height"] 
        }
    return region

# TODO: add support for not counterclockwise order (auto recognize in config init)
def normalize_screenshot(side: str, screen_data: np.array)->np.array:
    """
    Rotate the captured frame to match with counterclockwise order. 
    Top of the image should represent the lowest LED id.
    """
    if side == "bottom":
        return np.rot90(screen_data, 3)
    elif side == "right":
        return np.rot90(screen_data, 2)
    elif side == "top":
        return np.rot90(screen_data)
    return screen_data # side == "left" already in the correct order

def capture_screen(sct: mss, led_strip: dict)->np.array:
    monitor = sct.monitors[led_strip["monitor"]]
    region = identify_border_area(led_strip["side"], monitor)
    screenshot = sct.grab(region)
    return np.array(screenshot)

def process_screenshot(screenshot: np.array, led_strip: dict)->list:
    led_colors = []
    led_count = led_strip["led_count"]
    # rotate to uniform order
    normalized_screenshot = normalize_screenshot(led_strip["side"], screenshot)
    # calculate size of areas for each led
    screenshot_height = normalized_screenshot.shape[0]
    section_height = screenshot_height // led_count # floor division

    for i in range(led_count):
        # divide the screenshot into sections
        start_row = i*section_height
        end_row = (i+1)*section_height if i<led_count-1 else screenshot_height

        screenshot_section = normalized_screenshot[start_row:end_row]

        # calculate dominant color
        # TODO: add diffrent ways of chosing a color
        section_dominant_color = get_dominant_color(screenshot_section, quality=1) #returned tuple of (r, g, b)
        led_colors.extend([int(value) for value in section_dominant_color[:3]])
    return led_colors

def compose_screen_data(sct: mss)->list:
    message = []
    # for each side
    for led_strip in LED_MAP:
        # capture relevant part of the screen
        screenshot = capture_screen(sct, led_strip)
        # split into segments and choose the dominant color
        message.extend(process_screenshot(screenshot, led_strip))
    return message