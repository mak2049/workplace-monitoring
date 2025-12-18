import cv2
import mediapipe as mp
import math
from PIL import ImageFont, ImageDraw, Image
import numpy as np

mp_drawing = mp.solutions.drawing_utils
mp_pose = mp.solutions.pose

# Функция для отрисовки текста с поддержкой кириллицы
def draw_text(img, text, pos, color=(0,255,0), size=32):
    img_pil = Image.fromarray(img)
    draw = ImageDraw.Draw(img_pil)
    try:
        font = ImageFont.truetype("arial.ttf", size)  # системный шрифт
    except:
        font = ImageFont.load_default()
    draw.text(pos, text, font=font, fill=color)
    return np.array(img_pil)

# Функция для вычисления угла между тремя точками
def calculate_angle(a, b, c):
    ax, ay = a
    bx, by = b
    cx, cy = c

    ab = (ax - bx, ay - by)
    cb = (cx - bx, cy - by)

    dot_product = ab[0]*cb[0] + ab[1]*cb[1]
    magnitude_ab = math.sqrt(ab[0]**2 + ab[1]**2)
    magnitude_cb = math.sqrt(cb[0]**2 + cb[1]**2)

    if magnitude_ab * magnitude_cb == 0:
        return 0

    cos_angle = dot_product / (magnitude_ab * magnitude_cb)
    angle = math.degrees(math.acos(min(1, max(-1, cos_angle))))
    return angle


cap = cv2.VideoCapture(0)  # 0 = встроенная вебка
with mp_pose.Pose(min_detection_confidence=0.7, min_tracking_confidence=0.7) as pose:
    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:
            break

        # Конвертация BGR -> RGB
        image = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        results = pose.process(image)

        # Обратно в BGR для отображения
        image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)

        if results.pose_landmarks:
            mp_drawing.draw_landmarks(image, results.pose_landmarks, mp_pose.POSE_CONNECTIONS)

            landmarks = results.pose_landmarks.landmark
            h, w, _ = image.shape

            # Берем точки (левое ухо, левое плечо, левое бедро)
            shoulder = (int(landmarks[mp_pose.PoseLandmark.LEFT_SHOULDER].x * w),
                        int(landmarks[mp_pose.PoseLandmark.LEFT_SHOULDER].y * h))
            ear = (int(landmarks[mp_pose.PoseLandmark.LEFT_EAR].x * w),
                   int(landmarks[mp_pose.PoseLandmark.LEFT_EAR].y * h))
            hip = (int(landmarks[mp_pose.PoseLandmark.LEFT_HIP].x * w),
                   int(landmarks[mp_pose.PoseLandmark.LEFT_HIP].y * h))

            # Угол ухо–плечо–бедро
            angle = calculate_angle(ear, shoulder, hip)

            # Проверка ещё и по уровню плеч (для большей точности)
            right_shoulder = (int(landmarks[mp_pose.PoseLandmark.RIGHT_SHOULDER].x * w),
                              int(landmarks[mp_pose.PoseLandmark.RIGHT_SHOULDER].y * h))
            shoulder_diff = abs(shoulder[1] - right_shoulder[1])  # разница по высоте плеч

            if angle < 145 or shoulder_diff > 40:
                image = draw_text(image, "Плохая осанка!", (50, 50), color=(255,0,0), size=40)
            else:
                image = draw_text(image, "Осанка норм", (50, 50), color=(0,255,0), size=40)

        cv2.imshow('Posture Checker', image)

        if cv2.waitKey(5) & 0xFF == 27:  # ESC для выхода
            break

cap.release()
cv2.destroyAllWindows()
