#pragma execution_character_set("utf-8")

#include "mediapipe_pose.h"
#include <QDebug>

MediaPipePose::MediaPipePose(QObject *parent)
    : QObject(parent)
{
}

void MediaPipePose::start()
{
    process.setProcessChannelMode(QProcess::MergedChannels);

    connect(&process, &QProcess::readyReadStandardOutput,
            this, [this]() {

                while (process.canReadLine()) {
                    QString line = QString::fromUtf8(process.readLine()).trimmed();

                    if (line == "NOTIFY_BAD_POSTURE")
                        emit badPostureDetected();
                    else if (line == "CLEAR_BAD_POSTURE")
                        emit badPostureCleared();
                    else if (line.startsWith("PRESENCE_TIME")) {
                        emit presenceTimeUpdated(line.split(' ')[1].toInt());
                    }
                }
            });

    process.start(
        "C:/Users/Admin/AppData/Local/Programs/Python/Python310/python.exe",
        { "C:/Qt/my_project/WorkplaceMonitorvvvv/mp_pose_stdout.py" }
        );
}

void MediaPipePose::sendCalibrate()
{
    if (process.state() == QProcess::Running)
        process.write("CALIBRATE\n");
}
