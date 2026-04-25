import cv2
import numpy as np
import serial
import time

# --- CHANGE THIS FOR YOUR LAPTOP ---
# Windows example: 'COM3'
# Linux/Mac: '/dev/ttyACM0'
PORT = 'COM9'

ser = None

try:
    ser = serial.Serial(PORT, 115200, timeout=0.1)

    ser.setDTR(False)
    time.sleep(1)
    ser.flushInput()
    ser.setDTR(True)
    time.sleep(2)

    print("Serial connected")

except Exception as e:
    print("Serial failed:", e)
    ser = None


cap = cv2.VideoCapture(1)

enemy_was_seen = False

# --- SAFE SEND WITH TIMEOUT ---
def send_command(cmd):
    if not ser:
        return

    try:
        ser.write(cmd)

        start_time = time.time()

        # wait max 0.5 sec for ACK
        while time.time() - start_time < 0.5:
            ack = ser.read()
            if ack == b'A':
                return

        print("No ACK received")

    except Exception as e:
        print("Serial error:", e)


while True:
    ret, frame = cap.read()
    if not ret:
        continue

    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # RED
    lower_red1 = np.array([0,120,70])
    upper_red1 = np.array([10,255,255])
    lower_red2 = np.array([170,120,70])
    upper_red2 = np.array([180,255,255])

    red_mask = cv2.inRange(hsv, lower_red1, upper_red1) + \
               cv2.inRange(hsv, lower_red2, upper_red2)

    # BLUE
    lower_blue = np.array([100,150,0])
    upper_blue = np.array([140,255,255])
    blue_mask = cv2.inRange(hsv, lower_blue, upper_blue)

    contours_red, _ = cv2.findContours(red_mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    contours_blue, _ = cv2.findContours(blue_mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    enemy_detected = False

    # --- RED ---
    for cnt in contours_red:
        if cv2.contourArea(cnt) > 800:
            x,y,w,h = cv2.boundingRect(cnt)

            cv2.rectangle(frame, (x,y), (x+w,y+h), (0,0,255), 2)
            cv2.putText(frame, "ENEMY", (x,y-10),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0,0,255), 2)

            enemy_detected = True
            break

    # --- BLUE ---
    for cnt in contours_blue:
        if cv2.contourArea(cnt) > 800:
            x,y,w,h = cv2.boundingRect(cnt)

            cv2.rectangle(frame, (x,y), (x+w,y+h), (255,0,0), 2)
            cv2.putText(frame, "ALLY", (x,y-10),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255,0,0), 2)

    # --- STATE LOGIC ---
    if enemy_detected and not enemy_was_seen:
        print("Enemy → STOP")
        send_command(b'C')

    elif not enemy_detected and enemy_was_seen:
        print("Enemy lost → SPIN")
        send_command(b'S')

    enemy_was_seen = enemy_detected

    cv2.imshow("Tracking", frame)

    if cv2.waitKey(1) & 0xFF == 27:
        break

    time.sleep(0.03)

cap.release()
cv2.destroyAllWindows()

if ser:
    ser.close()