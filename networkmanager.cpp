#include "networkmanager.h"
#include <QHostAddress>
#include <QDebug>
#include <QNetworkInterface>
#include <QFileInfo>

NetworkManager::NetworkManager(QObject *parent)
    : QObject{parent}
{
    instanceId = QRandomGenerator::global()->generate64();

    broadcastTimer = new QTimer(this);
    connect(broadcastTimer, &QTimer::timeout, this, &NetworkManager::sendBroadcastDatagram);
}

void NetworkManager::initSocket()
{
    udpSocket = new QUdpSocket(this);
    bool isBound = udpSocket->bind(QHostAddress::Any, 45454, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);

    if (!isBound) {
        qDebug() << "ERROR: Could not bind socket! This instance will not receive messages.";
    } else {
        qDebug() << "Socket bound successfully. Listening for messages...";
    }

    connect(udpSocket, &QUdpSocket::readyRead, this, &NetworkManager::processPendingDatagrams);

    broadcastTimer->start(5000);
}

void NetworkManager::sendBroadcastDatagram()
{
    QByteArray datagram = "CampusDrop_Hello:" + QByteArray::number(instanceId);
    udpSocket->writeDatagram(datagram, QHostAddress::Broadcast, 45454);
}

void NetworkManager::processPendingDatagrams()
{
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(int(udpSocket->pendingDatagramSize()));
        QHostAddress senderIp;

        udpSocket->readDatagram(datagram.data(), datagram.size(), &senderIp);

        QList<QByteArray> parts = datagram.split(':');
        if (parts.size() == 2 && parts.at(0) == "CampusDrop_Hello") {
            quint64 receivedId = parts.at(1).toULongLong();
            if (receivedId != instanceId) {
                emit newUserFound(receivedId, senderIp.toString());
            }
        }
        else if (parts.size() == 5 && parts.at(0) == "FILE_REQUEST") {
            quint64 senderId = parts.at(1).toULongLong();
            quint64 targetId = parts.at(2).toULongLong();

            if (targetId == instanceId) {
                QString fileName = QString::fromUtf8(parts.at(3));
                qint64 fileSize = parts.at(4).toLongLong();
                emit fileRequestReceived(senderId, fileName, fileSize);
            }
        }
        else if (parts.size() == 3 && (parts.at(0) == "FILE_ACCEPT" || parts.at(0) == "FILE_DECLINE")) {
            quint64 senderId = parts.at(1).toULongLong();
            quint64 targetId = parts.at(2).toULongLong();

            if (targetId == instanceId) {
                if (parts.at(0) == "FILE_ACCEPT") {
                    emit transferAccepted(senderId, senderIp);
                } else {
                    emit transferDeclined(senderId);
                }
            }
        }
    }
}

void NetworkManager::sendFileRequest(quint64 targetId, const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    QString fileName = fileInfo.fileName();
    qint64 fileSize = fileInfo.size();

    QByteArray datagram = "FILE_REQUEST:" + QByteArray::number(instanceId) + ":" + QByteArray::number(targetId) + ":" + fileName.toUtf8() + ":" + QByteArray::number(fileSize);

    udpSocket->writeDatagram(datagram, QHostAddress::Broadcast, 45454);
}

void NetworkManager::sendFileReply(quint64 targetId, bool accepted)
{
    QByteArray command = accepted ? "FILE_ACCEPT" : "FILE_DECLINE";
    QByteArray datagram = command + ":" + QByteArray::number(instanceId) + ":" + QByteArray::number(targetId);
    udpSocket->writeDatagram(datagram, QHostAddress::Broadcast, 45454);
}
