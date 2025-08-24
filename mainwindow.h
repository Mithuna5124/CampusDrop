#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "networkmanager.h"

#include <QMap>
#include <QDateTime>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

struct UserInfo {
    QString displayText;
    QDateTime lastSeen;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void onNewUserFound(quint64 id, const QString &ipAddress);
    void checkTimeouts();

private:
    Ui::MainWindow *ui;
    NetworkManager *networkManager;
    QMap<quint64, UserInfo> onlineUsers;
    QTimer *timeoutTimer;
};
#endif // MAINWINDOW_H
