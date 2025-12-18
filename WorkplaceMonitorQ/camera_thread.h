#ifndef CAMERA_THREAD_H
#define CAMERA_THREAD_H

#include <QThread>
#include <QImage>
#include <QElapsedTimer>
#include <opencv2/opencv.hpp>

class CameraThread : public QThread
{
    Q_OBJECT

public:
    explicit CameraThread(QObject *parent = nullptr);
    void stop();
    void calibrate();

signals:
    void frameProcessed(const QImage &image, bool postureBad);

protected:
    void run() override;

private:
    bool running = true;
    bool calibrated = false;

    cv::Point2f headSmooth;
    cv::Point2f shoulderSmooth;

    double referenceDistance = 0.0;
    double smoothDistance = 0.0;

    const double alpha = 0.2;          // сглаживание
    const double postureThreshold = 0.85;

    QElapsedTimer calibrationTimer;
    bool showCalibrationMessage = false;
};

#endif
