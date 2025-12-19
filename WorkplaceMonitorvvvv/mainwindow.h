#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QSoundEffect>

class MediaPipePose;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onCalibrateClicked();
    void onBadPostureWarning();
    void onBadPostureCleared();

#ifdef Q_OS_WIN
private:
    void embedOpenCVWindow();
#endif

private:
    Ui::MainWindow *ui;
    MediaPipePose *pose;
    QSystemTrayIcon *trayIcon;
    QSoundEffect *alertSound;

private slots:
    void onPresenceTimeUpdated(int seconds);

};

#endif
