#ifndef MEDIAPIPE_POSE_H
#define MEDIAPIPE_POSE_H

#include <QObject>
#include <QProcess>

class MediaPipePose : public QObject
{
    Q_OBJECT
public:
    explicit MediaPipePose(QObject *parent = nullptr);
    void start();
    void sendCalibrate();

signals:
    void badPostureDetected();
    void badPostureCleared();
    void presenceTimeUpdated(int seconds);

private:
    QProcess process;
};

#endif
