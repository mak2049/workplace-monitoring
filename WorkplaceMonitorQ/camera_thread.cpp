#include "camera_thread.h"

CameraThread::CameraThread(QObject *parent)
    : QThread(parent), running(true)
{
}

CameraThread::~CameraThread()
{
    stop();
    wait();
}

void CameraThread::stop()
{
    running = false;
}

void CameraThread::run()
{
    cap.open(0);
    if (!cap.isOpened())
        return;

    while (running)
    {
        cv::Mat frame;
        cap >> frame;

        if (frame.empty())
            continue;

        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

        QImage image(frame.data,
                     frame.cols,
                     frame.rows,
                     frame.step,
                     QImage::Format_RGB888);

        emit frameReady(image.copy());

        msleep(30); // ~30 FPS
    }

    cap.release();
}
