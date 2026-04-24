import cv2
import numpy as np
import serial
import time

PORT = 'COM5'

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


cap = cv2.VideoCapture(0)


def send_command(cmd):
    if not ser:
        return

    try:
        ser.write(cmd)

        start_time = time.time()
        while time.time() - start_time < 0.2:
            if ser.read() == b'A':
                return

    except Exception as e:
        print("Serial error:", e)


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
    target_y = None

    # --- RED DETECTION ---
    for cnt in contours_red:
        if cv2.contourArea(cnt) > 800:
            x,y,w,h = cv2.boundingRect(cnt)

            cv2.rectangle(frame, (x,y), (x+w,y+h), (0,0,255), 2)
            cv2.putText(frame, "ENEMY", (x,y-10),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0,0,255), 2)

            enemy_detected = True
            target_x = x + w // 2
            target_y = y + h // 2
            break

    # --- BLUE DETECTION ---
    for cnt in contours_blue:
        if cv2.contourArea(cnt) > 800:
            x,y,w,h = cv2.boundingRect(cnt)

            cv2.rectangle(frame, (x,y), (x+w,y+h), (255,0,0), 2)
            cv2.putText(frame, "ALLY", (x,y-10),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255,0,0), 2)

    # --- TRACKING LOGIC ---
    if enemy_detected and target_x is not None and target_y is not None:

        # Center
        center_x = frame.shape[1] // 2
        center_y = frame.shape[0] // 2

        error_x = target_x - center_x
        error_y = target_y - center_y

        tolerance = 40

        # -------- X AXIS --------
        if abs(error_x) < tolerance:
            send_command(b'C')
        elif error_x > 0:
            send_command(b'R')
        else:
            send_command(b'L')

        # -------- Y AXIS --------
        if abs(error_y) < tolerance:
            send_command(b'Y')
        elif error_y > 0:
            send_command(b'D')
        else:
            send_command(b'U')

        print(f"X error: {error_x} | Y error: {error_y}")

    else:
        send_command(b'S')
        print("Searching...")

    cv2.imshow("Tracking", frame)

    if cv2.waitKey(1) & 0xFF == 27:
        break

    time.sleep(0.03)

cap.release()
cv2.destroyAllWindows()

if ser:
    ser.close()