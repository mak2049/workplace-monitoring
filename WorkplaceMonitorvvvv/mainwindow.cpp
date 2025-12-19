#pragma execution_character_set("utf-8")

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mediapipe_pose.h"

#include <QTimer>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    pose(new MediaPipePose(this)),
    trayIcon(new QSystemTrayIcon(this)),
    alertSound(new QSoundEffect(this))
{
    ui->setupUi(this);

    trayIcon->setIcon(QIcon::fromTheme("dialog-warning"));
    trayIcon->show();

    alertSound->setSource(QUrl::fromLocalFile(
        QCoreApplication::applicationDirPath() + "/sounds/alert.wav"
        ));
    alertSound->setVolume(0.9);

    connect(ui->btnCalibrate, &QPushButton::clicked,
            this, &MainWindow::onCalibrateClicked);

    connect(pose, &MediaPipePose::badPostureDetected,
            this, &MainWindow::onBadPostureWarning);

    connect(pose, &MediaPipePose::badPostureCleared,
            this, &MainWindow::onBadPostureCleared);

    connect(pose, &MediaPipePose::presenceTimeUpdated,
            this, &MainWindow::onPresenceTimeUpdated);


    pose->start();

#ifdef Q_OS_WIN
    QTimer::singleShot(2000, this, &MainWindow::embedOpenCVWindow);
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onCalibrateClicked()
{
    pose->sendCalibrate();
}

void MainWindow::onBadPostureWarning()
{
    trayIcon->showMessage(
        "osanka",
        "osanka bad 5s",
        QSystemTrayIcon::Warning,
        3000
        );

    if (alertSound->isLoaded())
        alertSound->play();
}

void MainWindow::onBadPostureCleared()
{

}

#ifdef Q_OS_WIN
void MainWindow::embedOpenCVWindow()
{
    HWND wnd = nullptr;
    for (int i = 0; i < 50; ++i) {
        wnd = FindWindowA(nullptr, "Posture Monitor");
        if (wnd) break;
        Sleep(100);
    }

    if (!wnd) return;

    HWND labelWnd = (HWND)ui->labelVideo->winId();
    SetParent(wnd, labelWnd);

    LONG style = GetWindowLong(wnd, GWL_STYLE);
    style &= ~(WS_CAPTION | WS_THICKFRAME);
    SetWindowLong(wnd, GWL_STYLE, style);

    RECT r;
    GetClientRect(labelWnd, &r);
    MoveWindow(wnd, 0, 0,
               r.right - r.left,
               r.bottom - r.top,
               TRUE);
}

void MainWindow::onPresenceTimeUpdated(int seconds)
{
    int h = seconds / 3600;
    int m = (seconds % 3600) / 60;
    int s = seconds % 60;

    ui->labelWorkTime->setText(
        QString("Time at the workplace: %1:%2:%3")
            .arg(h, 2, 10, QChar('0'))
            .arg(m, 2, 10, QChar('0'))
            .arg(s, 2, 10, QChar('0'))
        );
}

#endif

void MainWindow::on_tableView_activated(const QModelIndex &index)
{

}

