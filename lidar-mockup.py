import random
import time

ANGLE_STEP = 1 # deg
TIME_STEP = 1 # s

angle = 0
while True:
    distance = 1000 + random.randrange(0, 9000)
    angle += ANGLE_STEP
    print(angle, distance, time.time_ns() // 1_000_000)

    time.sleep(TIME_STEP)
