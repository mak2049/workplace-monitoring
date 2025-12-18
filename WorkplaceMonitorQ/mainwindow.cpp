#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    cameraThread(new CameraThread(this))
{
    ui->setupUi(this);

    connect(cameraThread, &CameraThread::frameProcessed,
            this, &MainWindow::onFrame);

    connect(ui->btnCalibrate, &QPushButton::clicked,
            cameraThread, &CameraThread::calibrate);

    cameraThread->start();
}

MainWindow::~MainWindow()
{
    cameraThread->stop();
    cameraThread->wait();
    delete ui;
}

void MainWindow::onFrame(const QImage &img, bool bad)
{
    QImage display = img.copy();

    QPainter painter(&display);
    painter.setFont(QFont("Arial", 24));
    painter.setPen(bad ? Qt::red : Qt::green);
    painter.drawText(20, 40, bad ? "BAD POSTURE" : "GOOD POSTURE");

    ui->labelVideo->setPixmap(
        QPixmap::fromImage(display).scaled(
            ui->labelVideo->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            )
        );
}
