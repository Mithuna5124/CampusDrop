#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "networkmanager.h"
#include <QListWidgetItem>
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
    void onFileRequestReceived(quint64 senderId, const QString &fileName, qint64 fileSize);

private slots:
    void onUserDoubleClicked(QListWidgetItem *item);
    void onTransferAccepted(quint64 senderId, const QHostAddress &senderIp);
    void onTransferDeclined(quint64 senderId);

private:
    Ui::MainWindow *ui;
    NetworkManager *networkManager;
    QMap<quint64, UserInfo> onlineUsers;
    QTimer *timeoutTimer;
    quint64 lastSenderId = 0;
    QString lastFileToSendPath;

};
#endif // MAINWINDOW_H
