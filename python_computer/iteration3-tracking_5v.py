import cv2
import numpy as np
import serial
import time

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

last_command = None

def send_command(cmd):
    if not ser:
        return

    try:
        ser.write(cmd)

        start_time = time.time()
        while time.time() - start_time < 0.5:
            ack = ser.read()
            if ack == b'A':
                return

        print("No ACK")

    except Exception as e:
        print("Serial error:", e)


def send_if_new(cmd):
    global last_command
    if cmd != last_command:
        send_command(cmd)
        last_command = cmd


while True:
    ret, frame = cap.read()
    if not ret:
        continue

    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # --- RED ---
    lower_red1 = np.array([0,120,70])
    upper_red1 = np.array([10,255,255])
    lower_red2 = np.array([170,120,70])
    upper_red2 = np.array([180,255,255])

    red_mask = cv2.inRange(hsv, lower_red1, upper_red1) + \
               cv2.inRange(hsv, lower_red2, upper_red2)

    # --- BLUE ---
    lower_blue = np.array([100,150,0])
    upper_blue = np.array([140,255,255])

    blue_mask = cv2.inRange(hsv, lower_blue, upper_blue)

    contours_red, _ = cv2.findContours(red_mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    contours_blue, _ = cv2.findContours(blue_mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    enemy_detected = False
    target_x = None

    # --- RED DETECTION ---
    for cnt in contours_red:
        if cv2.contourArea(cnt) > 800:
            x,y,w,h = cv2.boundingRect(cnt)

            cv2.rectangle(frame, (x,y), (x+w,y+h), (0,0,255), 2)
            cv2.putText(frame, "ENEMY", (x,y-10),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0,0,255), 2)

            enemy_detected = True
            target_x = x + w // 2
            break

    # --- BLUE DETECTION ---
    for cnt in contours_blue:
        if cv2.contourArea(cnt) > 800:
            x,y,w,h = cv2.boundingRect(cnt)

            cv2.rectangle(frame, (x,y), (x+w,y+h), (255,0,0), 2)
            cv2.putText(frame, "ALLY", (x,y-10),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255,0,0), 2)

    # --- TRACKING LOGIC ---
    if enemy_detected and target_x is not None:
        frame_center = frame.shape[1] // 2
        error = target_x - frame_center

        tolerance = 40

        if abs(error) < tolerance:
            send_if_new(b'C')
            print("Centered → STOP")

        elif error > 0:
            send_if_new(b'R')
            print("RIGHT")

        else:
            send_if_new(b'L')
            print("LEFT")

    else:
        send_if_new(b'S')
        print("Searching...")

    cv2.imshow("Tracking", frame)

    if cv2.waitKey(1) & 0xFF == 27:
        break

    time.sleep(0.03)

cap.release()
cv2.destroyAllWindows()

if ser:
    ser.close()