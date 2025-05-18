#!../venv/bin/python3

import time
from adafruit_servokit import ServoKit

kit = ServoKit(channels=16)
while True:
    kit.continuous_servo[0].throttle = 0.0
    kit.continuous_servo[1].throttle = 0.0
    time.sleep(1)
