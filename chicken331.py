import cv2
import numpy as np
import serial
import time

PORT_rotation = '/dev/ttyACM2'
PORT_trigger = '/dev/ttyACM0'

ser = None
ser_1 = None

try:
    ser = serial.Serial(PORT_rotation, 115200, timeout=0)
    ser_1 = serial.Serial(PORT_trigger, 115200, timeout=0)
    time.sleep(2)
    print("Serial connected")
except Exception as e:
    print("Serial failed:", e)

cap = cv2.VideoCapture(0)
cap.set(3, 320)
cap.set(4, 240)
cap.set(cv2.CAP_PROP_BUFFERSIZE, 1)

# ---- STATE ----
last_send_time = 0
send_interval = 0.05

last_x = None
last_y = None

last_fire = False
last_fire_time = 0
cooldown = 0.4   # lidt længere for mekanik

def send(x, y):
    if ser:
        msg = f"{x},{y}\n"
        ser.write(msg.encode())

while True:
    ret, frame = cap.read()
    if not ret:
        continue

    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # RED detection
    lower_red1 = np.array([0,120,70])
    upper_red1 = np.array([10,255,255])
    lower_red2 = np.array([170,120,70])
    upper_red2 = np.array([180,255,255])

    red_mask = cv2.inRange(hsv, lower_red1, upper_red1) + \
               cv2.inRange(hsv, lower_red2, upper_red2)

    contours, _ = cv2.findContours(
        red_mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE
    )

    enemy = False
    tx, ty = None, None

    for c in contours:
        if cv2.contourArea(c) > 800:
            x, y, w, h = cv2.boundingRect(c)
            tx = x + w // 2
            ty = y + h // 2
            enemy = True
            break

    cx = frame.shape[1] // 2
    cy = frame.shape[0] // 2

    if enemy:
        error_x = int((tx - cx) / 1.5)
        error_y = int(ty - cy)
    else:
        error_x = 0
        error_y = 0

    now = time.time()

    # ---- SEND ROTATION ----
    if (
        ser
        and (now - last_send_time > send_interval)
        and (error_x != last_x or error_y != last_y)
    ):
        send(error_x, error_y)
        last_send_time = now
        last_x = error_x
        last_y = error_y

    # ---- CENTER CHECK ----
    in_center = (-5 < error_x < 5) and (-5 < error_y < 5)

    # ---- TRIGGER ----
    if in_center:
        if (not last_fire) and (now - last_fire_time > cooldown):
            if ser_1:
                ser_1.write(b'1\n')
            last_fire = True
            last_fire_time = now
    else:
        last_fire = False

    cv2.imshow("frame", frame)
    if cv2.waitKey(1) == 27:
        break

cap.release()
cv2.destroyAllWindows()

if ser:
    ser.close()
if ser_1:
    ser_1.close()
