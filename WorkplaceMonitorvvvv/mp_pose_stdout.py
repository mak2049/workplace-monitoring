import cv2
import mediapipe as mp
import numpy as np
from collections import deque
import time
import sys
import threading

WINDOW_NAME = "Posture Monitor"

SMOOTH_WINDOW = 5
ANGLE_DELTA = 8
SHOULDER_DELTA = 12
BAD_POSTURE_TIMEOUT = 5.0

mp_pose = mp.solutions.pose
mp_draw = mp.solutions.drawing_utils

angle_buf = deque(maxlen=SMOOTH_WINDOW)
shoulder_buf = deque(maxlen=SMOOTH_WINDOW)

reference_angle = None
reference_shoulder = None
last_calibration_time = 0

current_angle = None
current_shoulder = None

bad_posture_start_time = None
notification_sent = False
calibrate_requested = False

# ====== ВРЕМЯ ПРИСУТСТВИЯ ======
presence_start_time = None
total_presence_time = 0.0
last_report_time = 0


def stdin_listener():
    global calibrate_requested
    while True:
        line = sys.stdin.readline()
        if not line:
            break
        if line.strip() == "CALIBRATE":
            calibrate_requested = True
            print("CALIBRATION RECEIVED", flush=True)

threading.Thread(target=stdin_listener, daemon=True).start()


def smooth(val, buf):
    buf.append(val)
    return sum(buf) / len(buf)


def calculate_angle(a, b, c):
    a, b, c = np.array(a), np.array(b), np.array(c)
    ba = a - b
    bc = c - b
    cos = np.dot(ba, bc) / (np.linalg.norm(ba)*np.linalg.norm(bc) + 1e-6)
    return np.degrees(np.arccos(np.clip(cos, -1.0, 1.0)))


cap = cv2.VideoCapture(0)
cap.set(3, 640)
cap.set(4, 480)

cv2.namedWindow(WINDOW_NAME, cv2.WINDOW_NORMAL)
cv2.resizeWindow(WINDOW_NAME, 640, 480)

print("WINDOW CREATED", flush=True)

with mp_pose.Pose(
    min_detection_confidence=0.6,
    min_tracking_confidence=0.6
) as pose:

    while True:
        ret, frame = cap.read()
        if not ret:
            break

        now = time.time()
        h, w, _ = frame.shape
        rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        res = pose.process(rgb)

        status = "NO PERSON"
        color = (160, 160, 160)
        person_present = False

        if res.pose_landmarks:
            person_present = True
            lm = res.pose_landmarks.landmark

            ear = (int(lm[7].x*w), int(lm[7].y*h))
            sh  = (int(lm[11].x*w), int(lm[11].y*h))
            hip = (int(lm[23].x*w), int(lm[23].y*h))
            rsh = (int(lm[12].x*w), int(lm[12].y*h))

            current_angle = smooth(calculate_angle(ear, sh, hip), angle_buf)
            current_shoulder = smooth(abs(sh[1] - rsh[1]), shoulder_buf)

            mp_draw.draw_landmarks(frame, res.pose_landmarks,
                                   mp_pose.POSE_CONNECTIONS)

            if reference_angle is None:
                status = "PRESS CALIBRATE"
                color = (0, 255, 255)

            else:
                bad = (
                    current_angle < reference_angle - ANGLE_DELTA or
                    current_shoulder > reference_shoulder + SHOULDER_DELTA
                )

                if bad:
                    status = "BAD POSTURE"
                    color = (0, 0, 255)

                    if bad_posture_start_time is None:
                        bad_posture_start_time = now
                        notification_sent = False

                    if (now - bad_posture_start_time >= BAD_POSTURE_TIMEOUT
                            and not notification_sent):
                        print("NOTIFY_BAD_POSTURE", flush=True)
                        notification_sent = True

                else:
                    status = "GOOD POSTURE"
                    color = (0, 255, 0)

                    if notification_sent:
                        print("CLEAR_BAD_POSTURE", flush=True)

                    bad_posture_start_time = None
                    notification_sent = False

        # ===== ПОДСЧЁТ ВРЕМЕНИ =====
        if person_present:
            if presence_start_time is None:
                presence_start_time = now
        else:
            if presence_start_time is not None:
                total_presence_time += now - presence_start_time
                presence_start_time = None

        if now - last_report_time >= 1:
            total = total_presence_time
            if presence_start_time:
                total += now - presence_start_time
            print(f"PRESENCE_TIME {int(total)}", flush=True)
            last_report_time = now

        # ===== КАЛИБРОВКА =====
        if calibrate_requested and current_angle is not None:
            reference_angle = current_angle
            reference_shoulder = current_shoulder
            last_calibration_time = now
            calibrate_requested = False

        cv2.putText(frame, status, (30, 40),
                    cv2.FONT_HERSHEY_SIMPLEX, 1.2, color, 3)

        if time.time() - last_calibration_time < 2:
            cv2.putText(frame, "CALIBRATION SAVED",
                        (30, 90),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.9,
                        (0,255,0), 2)

        cv2.imshow(WINDOW_NAME, frame)

        if cv2.waitKey(1) & 0xFF == 27:
            break

cap.release()
cv2.destroyAllWindows()
