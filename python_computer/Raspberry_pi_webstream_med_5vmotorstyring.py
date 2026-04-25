import socket
import cv2
import numpy as np
import struct

HOST = "192.168.1.106"
PORT = 5000

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect((HOST, PORT))

data = b""
payload_size = struct.calcsize(">L")

# --------------------
# MEMORY (IMPORTANT FIX)
# --------------------
last_direction = None

while True:

    # --- GET SIZE ---
    while len(data) < payload_size:
        packet = client.recv(4096)
        if not packet:
            break
        data += packet

    packed_size = data[:payload_size]
    data = data[payload_size:]
    msg_size = struct.unpack(">L", packed_size)[0]

    # --- GET FRAME ---
    while len(data) < msg_size:
        data += client.recv(4096)

    frame_data = data[:msg_size]
    data = data[msg_size:]

    frame = cv2.imdecode(np.frombuffer(frame_data, np.uint8), cv2.IMREAD_COLOR)

    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # --- RED ---
    lower_red1 = np.array([0,120,70])
    upper_red1 = np.array([10,255,255])
    lower_red2 = np.array([170,120,70])
    upper_red2 = np.array([180,255,255])

    red_mask = cv2.inRange(hsv, lower_red1, upper_red1) + \
               cv2.inRange(hsv, lower_red2, upper_red2)

    contours, _ = cv2.findContours(red_mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    enemy_detected = False
    target_x = None

    for cnt in contours:
        if cv2.contourArea(cnt) > 800:
            x, y, w, h = cv2.boundingRect(cnt)
            target_x = x + w // 2
            enemy_detected = True

            cv2.rectangle(frame, (x,y), (x+w,y+h), (0,0,255), 2)
            break

    # --- LOGIC ---
    if enemy_detected and target_x is not None:

        center_x = frame.shape[1] // 2
        error = target_x - center_x

        tolerance = 40

        if abs(error) < tolerance:
            client.send(b'C')
        elif error > 0:
            client.send(b'R')
            last_direction = "RIGHT"
        else:
            client.send(b'L')
            last_direction = "LEFT"

    else:
        # 🧠 SMART SEARCH (NO ZIGZAG)
        if last_direction == "RIGHT":
            client.send(b'R')
        elif last_direction == "LEFT":
            client.send(b'L')
        else:
            client.send(b'S')

    cv2.imshow("Stream", frame)

    if cv2.waitKey(1) == 27:
        break

client.close()
cv2.destroyAllWindows()