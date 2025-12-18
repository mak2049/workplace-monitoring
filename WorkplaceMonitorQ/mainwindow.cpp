#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainter>
#include <QDebug>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , cameraThread(new CameraThread(this))
{
    ui->setupUi(this);

    if (!faceCascade.load("haarcascade_frontalface_default.xml")) {
        qDebug() << "Face cascade not loaded";
    }

    connect(cameraThread, &CameraThread::frameReady,
            this, &MainWindow::onFrameReceived);

    connect(ui->btnCalibrate, &QPushButton::clicked,
            this, &MainWindow::onCalibrateClicked);

    cameraThread->start();
}

MainWindow::~MainWindow()
{
    cameraThread->stop();
    cameraThread->wait();
    delete ui;
}

void MainWindow::onCalibrateClicked()
{
    isCalibrating = true;
}

void MainWindow::onFrameReceived(const QImage &image)
{
    // IMPORTANT: copy image for thread safety
    QImage displayImage = image.copy();

    frameCounter++;

    // Convert QImage -> cv::Mat (RGB)
    cv::Mat frame(
        displayImage.height(),
        displayImage.width(),
        CV_8UC3,
        displayImage.bits(),
        displayImage.bytesPerLine()
        );

    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_RGB2GRAY);

    std::vector<cv::Rect> faces;

    // Heavy detection only every 5th frame
    if (!faceCascade.empty() && frameCounter % 5 == 0) {
        faceCascade.detectMultiScale(gray, faces, 1.1, 3);
    }

    if (!faces.empty())
    {
        cv::Rect face = faces[0];

        // Head (face center)
        cv::Point head(
            face.x + face.width / 2,
            face.y + face.height / 2
            );

        // Approximate shoulders
        cv::Point leftShoulder(
            face.x,
            face.y + face.height * 2
            );

        cv::Point rightShoulder(
            face.x + face.width,
            face.y + face.height * 2
            );

        // Draw markers
        cv::circle(frame, head, 5, cv::Scalar(255, 0, 0), -1);
        cv::circle(frame, leftShoulder, 5, cv::Scalar(0, 0, 255), -1);
        cv::circle(frame, rightShoulder, 5, cv::Scalar(0, 0, 255), -1);

        // Calibration step
        if (isCalibrating)
        {
            reference.head = head;
            reference.leftShoulder = leftShoulder;
            reference.rightShoulder = rightShoulder;

            isCalibrated = true;
            isCalibrating = false;
        }

        // Posture check
        if (isCalibrated)
        {
            int headShift =
                std::abs(head.y - reference.head.y);

            int currentShoulderY =
                (leftShoulder.y + rightShoulder.y) / 2;

            int referenceShoulderY =
                (reference.leftShoulder.y + reference.rightShoulder.y) / 2;

            int shoulderShift =
                std::abs(currentShoulderY - referenceShoulderY);

            QString statusText;
            QColor statusColor;

            if (headShift > 20 || shoulderShift > 20) {
                statusText = "BAD POSTURE";
                statusColor = Qt::red;
            } else {
                statusText = "GOOD POSTURE";
                statusColor = Qt::green;
            }

            QPainter painter(&displayImage);
            painter.setPen(statusColor);
            painter.setFont(QFont("Arial", 24));
            painter.drawText(20, 40, statusText);
        }
    }

    ui->labelVideo->setPixmap(
        QPixmap::fromImage(displayImage).scaled(
            ui->labelVideo->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            )
        );
}
