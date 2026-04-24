import cv2
import socketio
import time
import numpy as np

SERVER_IP = "192.168.1.110"  # din laptop IP

sio = socketio.Client()
sio.connect(f"http://{SERVER_IP}:5000")

uid = "12345"
sio.emit("joinRoom", uid)

cap = cv2.VideoCapture(0)

if not cap.isOpened():
    print("Camera not found")
    exit()

print("Streaming started...")

while True:
    ret, frame = cap.read()
    if not ret:
        continue

    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # ---------- RED ----------
    lower_red1 = np.array([0,120,70])
    upper_red1 = np.array([10,255,255])
    lower_red2 = np.array([170,120,70])
    upper_red2 = np.array([180,255,255])

    red_mask = cv2.inRange(hsv, lower_red1, upper_red1) + \
               cv2.inRange(hsv, lower_red2, upper_red2)

    contours_red, _ = cv2.findContours(red_mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    for cnt in contours_red:
        if cv2.contourArea(cnt) > 800:
            x,y,w,h = cv2.boundingRect(cnt)
            cv2.rectangle(frame, (x,y), (x+w,y+h), (0,0,255), 2)
            cv2.putText(frame, "ENEMY", (x,y-10),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0,0,255), 2)
            break

    # 🔥 resize for performance
    frame = cv2.resize(frame, (320, 240))

    # encode
    _, buffer = cv2.imencode('.jpg', frame, [
        int(cv2.IMWRITE_JPEG_QUALITY), 50
    ])

    # send
    sio.emit("sendFrame", (uid, buffer.tobytes()))

    time.sleep(0.1)
