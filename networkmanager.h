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
    void sendFileRequest(quint64 targetId, const QString &filePath);
    void sendFileReply(quint64 targetId, bool accepted);

signals:
    void newUserFound(quint64 id, const QString &ipAddress);
    void fileRequestReceived(quint64 senderId, const QString &fileName, qint64 fileSize);
    void transferAccepted(quint64 senderId, const QHostAddress &senderIp);
    void transferDeclined(quint64 senderId);

private slots:
    void processPendingDatagrams();

private:
    QUdpSocket *udpSocket = nullptr;
    QTimer *broadcastTimer = nullptr;
    quint64 instanceId;
};

#endif // NETWORKMANAGER_H
