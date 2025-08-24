#include "networkmanager.h"
#include <QHostAddress>
#include <QDebug>
#include <QNetworkInterface>

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
    }
}
