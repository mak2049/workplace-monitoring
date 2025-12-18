#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

#include "camera_thread.h"

#include <opencv2/opencv.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct PostureReference
{
    cv::Point head;
    cv::Point leftShoulder;
    cv::Point rightShoulder;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onFrameReceived(const QImage &image);
    void onCalibrateClicked();

private:
    Ui::MainWindow *ui;
    CameraThread *cameraThread;

    bool isCalibrated = false;
    bool isCalibrating = false;

    int frameCounter = 0;

    PostureReference reference;

    cv::CascadeClassifier faceCascade;
};

#endif // MAINWINDOW_H
