import cv2
import numpy as np
import socketio
import serial
import time

# --------------------
# SOCKET (WEB STREAM)
# --------------------
SERVER_IP = "192.168.1.110"

sio = socketio.Client()
sio.connect(f"http://{SERVER_IP}:5000")

uid = "12345"
sio.emit("joinRoom", uid)

# --------------------
# ARDUINO
# --------------------
PORT = "/dev/ttyACM0"

ser = None
try:
    ser = serial.Serial(PORT, 115200, timeout=0.1)
    time.sleep(2)
    print("Arduino connected")
except Exception as e:
    print("Arduino failed:", e)

def send(cmd):
    if ser:
        ser.write(cmd.encode())

# --------------------
# CAMERA
# --------------------
cap = cv2.VideoCapture(0)
cap.set(3, 320)
cap.set(4, 240)

last_packet = None

# --------------------
# LOOP
# --------------------
while True:
    ret, frame = cap.read()
    if not ret:
        continue

    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # ---------------- RED ----------------
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
    tol = 40

    # ---------------- X ----------------
    if enemy:
        if abs(tx - cx) < tol:
            cmd_x = 'C'
        elif tx > cx:
            cmd_x = 'R'
        else:
            cmd_x = 'L'
    else:
        cmd_x = 'S'

    # ---------------- Y ----------------
    if enemy:
        if abs(ty - cy) < tol:
            cmd_y = 'Y'
        elif ty > cy:
            cmd_y = 'D'
        else:
            cmd_y = 'U'
    else:
        cmd_y = 'Y'

    # ---------------- SEND TO ARDUINO ----------------
    packet = cmd_x + cmd_y

    if packet != last_packet:
        send(packet)
        last_packet = packet

    # ---------------- STREAM ----------------
    frame = cv2.resize(frame, (320, 240))

    _, buffer = cv2.imencode('.jpg', frame, [
        int(cv2.IMWRITE_JPEG_QUALITY), 50
    ])

    sio.emit("sendFrame", (uid, buffer.tobytes()))

    time.sleep(0.03)
