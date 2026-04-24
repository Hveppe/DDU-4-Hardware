import cv2
import numpy as np
import serial
import time

PORT = '/dev/ttyACM0'

ser = None
try:
    ser = serial.Serial(PORT, 115200, timeout=0.1)
    time.sleep(2)
    print("Serial connected")
except Exception as e:
    print("Serial failed:", e)

cap = cv2.VideoCapture(0)
cap.set(3, 320)
cap.set(4, 240)

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

    contours, _ = cv2.findContours(red_mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    enemy = False
    tx, ty = None, None

    for c in contours:
        if cv2.contourArea(c) > 800:
            x, y, w, h = cv2.boundingRect(c)
            tx = x + w//2
            ty = y + h//2
            enemy = True
            break

    cx = frame.shape[1] // 2
    cy = frame.shape[0] // 2

    if enemy:
        error_x = tx - cx
        error_y = ty - cy
    else:
        error_x = 0
        error_y = 0

    # send raw errors (THIS IS THE MAGIC)
    send(error_x/1.5, error_y)

    print(error_x, error_y)

    cv2.imshow("frame", frame)
    if cv2.waitKey(1) == 27:
        break

cap.release()
cv2.destroyAllWindows()
if ser:
    ser.close()
