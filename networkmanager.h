#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QRandomGenerator>

class NetworkManager : public QObject
{
    Q_OBJECT
public:
    explicit NetworkManager(QObject *parent = nullptr);

    void initSocket();
    void sendBroadcastDatagram();

signals:
    void newUserFound(quint64 id, const QString &ipAddress);

private slots:
    void processPendingDatagrams();

private:
    QUdpSocket *udpSocket = nullptr;
    QTimer *broadcastTimer = nullptr;
    quint64 instanceId;
};

#endif // NETWORKMANAGER_H
