import cv2
import numpy as np
import serial
import time

#serial forbindelse til Arduino
ser = serial.Serial("/dev/ttyACM0",9600)
time.sleep(2) #giv arduino tid til at starte

cap = cv2.VideoCapture(0)

frame_width = 640
frame_center = frame_width // 2

while True:
	ret,frame = cap.read()
	if not ret:
		break
	hsv =cv2.cvtColor(frame,cv2.COLOR_BGR2HSV)
	
	#red mask
	lower_red1 = np.array([0,120,70])
	upper_red1 = np.array([10,255,255])
	lower_red2 = np.array([170,120,70])
	upper_red2 = np.array([180,255,255])

	red_mask = cv2.inRange(hsv,lower_red1,upper_red1) +\
		   cv2.inRange(hsv,lower_red2,upper_red2)

	#blue mask
	lower_blue = np.array([100,150,0])
	upper_blue = np.array([140,255,255])
	blue_mask = cv2.inRange(hsv,lower_blue,upper_blue)

	#find konturer
	contours_red, _ = cv2.findContours(red_mask,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
	contours_blue, _ = cv2.findContours(blue_mask,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)

	target_found =False

	#Rød detektion + tracking
	for cnt in contours_red:
		if cv2.contourArea(cnt) > 800:
			x,y,w,h = cv2.boundingRect(cnt)
			center_x=x+2//2

			#Tegn boks (rød)			
			cv2.rectangle(frame, (x,y), (x+w,y+h), (0,0,255),2)
			cv2.putText(frame,"ENEMY",(x,y-10),cv2.FONT_HERSHEY_SIMPLEX,0.5,(0,0,255),2)

			#Tracking kun rød
			if not target_found:
				error = center_x - frame_center
				if error < -50:
					ser.write(b"l")
				elif error > 50:
					ser.write(b"R")
				else:
					ser.write(b"c")
				target_found = True


	# blå detection
	for cnt in contours_blue:
		if cv2.contourArea(cnt) > 500:
			x,y,w,h = cv2.boundingRect(cnt)
			cv2.rectangle(frame, (x,y), (x+w,y+h), (255,0,0),2)
			cv2.putText(frame,"ALLY",(x,y-10),cv2.FONT_HERSHEY_SIMPLEX,0.5,(255,0,0),2)

	if not target_found:
		ser.write(b"c")

	cv2.imshow("Tracking",frame)
	if cv2.waitKey(1) & 0xFF == 27:
		break
	time.sleep(0.05)
cap.release()
cv2.destroyAllWindows()
ser.close()