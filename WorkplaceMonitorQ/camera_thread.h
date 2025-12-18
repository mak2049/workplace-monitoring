#ifndef CAMERA_THREAD_H
#define CAMERA_THREAD_H

#include <QThread>
#include <QImage>
#include <atomic>

#include <opencv2/opencv.hpp>

class CameraThread : public QThread
{
    Q_OBJECT

public:
    explicit CameraThread(QObject *parent = nullptr);
    ~CameraThread();

    void stop();

signals:
    void frameReady(const QImage &image);

protected:
    void run() override;

private:
    std::atomic<bool> running;
    cv::VideoCapture cap;
};

#endif // CAMERA_THREAD_H
