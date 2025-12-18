#include "camera_thread.h"
#include <cmath>

CameraThread::CameraThread(QObject *parent)
    : QThread(parent)
{
}

void CameraThread::stop()
{
    running = false;
}

void CameraThread::calibrate()
{
    if (smoothDistance > 0) {
        referenceDistance = smoothDistance;
        calibrated = true;
        calibrationTimer.restart();
        showCalibrationMessage = true;
    }
}

void CameraThread::run()
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
        return;

    cv::CascadeClassifier faceCascade;
    faceCascade.load("haarcascade_frontalface_default.xml");

    bool firstFrame = true;

    while (running)
    {
        cv::Mat frame;
        cap >> frame;
        if (frame.empty())
            continue;

        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        std::vector<cv::Rect> faces;
        faceCascade.detectMultiScale(gray, faces, 1.1, 3);

        bool badPosture = false;

        if (!faces.empty())
        {
            cv::Rect face = faces[0];

            cv::Point2f head(
                face.x + face.width / 2.0f,
                face.y + face.height / 2.0f
                );

            cv::Point2f shoulders(
                face.x + face.width / 2.0f,
                face.y + face.height * 1.8f
                );

            if (firstFrame) {
                headSmooth = head;
                shoulderSmooth = shoulders;
                firstFrame = false;
            } else {
                headSmooth = alpha * head + (1 - alpha) * headSmooth;
                shoulderSmooth = alpha * shoulders + (1 - alpha) * shoulderSmooth;
            }

            double dist = std::hypot(
                headSmooth.x - shoulderSmooth.x,
                headSmooth.y - shoulderSmooth.y
                );

            smoothDistance = alpha * dist + (1 - alpha) * smoothDistance;

            if (calibrated) {
                badPosture = (smoothDistance < referenceDistance * postureThreshold);
            }

            // ----- ÐÈÑÓÅÌ -----
            cv::circle(frame, headSmooth, 6, {255, 0, 0}, -1);
            cv::circle(frame, shoulderSmooth, 6, {0, 255, 255}, -1);
            cv::line(frame, headSmooth, shoulderSmooth, {255, 255, 0}, 2);
        }

        if (showCalibrationMessage) {
            if (calibrationTimer.elapsed() < 2000) {
                cv::putText(frame, "Calibration successful",
                            {20, 40},
                            cv::FONT_HERSHEY_SIMPLEX,
                            1.0,
                            {0, 255, 0}, 2);
            } else {
                showCalibrationMessage = false;
            }
        }

        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
        QImage img(frame.data, frame.cols, frame.rows,
                   frame.step, QImage::Format_RGB888);

        emit frameProcessed(img.copy(), badPosture);
        msleep(15);
    }

    cap.release();
}
