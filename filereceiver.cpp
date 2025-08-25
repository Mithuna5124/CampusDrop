#include "filereceiver.h"
#include <QDebug>
#include <QStandardPaths>

FileReceiver::FileReceiver(const QString &fileName, QObject *parent)
    : QObject{parent}
{
    QString savePath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    incomingFile.setFileName(savePath + "/" + fileName);

    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection, this, &FileReceiver::onNewConnection);
}

void FileReceiver::startListening()
{
    if (!tcpServer->listen(QHostAddress::Any, 54545)) {
        qDebug() << "Receiver: Server could not start!";
    } else {
        qDebug() << "Receiver: Server started! Listening for connections on port 54545...";
    }
}

void FileReceiver::onNewConnection()
{
    tcpSocket = tcpServer->nextPendingConnection();

    connect(tcpSocket, &QTcpSocket::readyRead, this, &FileReceiver::onReadyRead);

    qDebug() << "Receiver: A client has connected!";

    if (!incomingFile.open(QIODevice::WriteOnly)) {
        qDebug() << "Receiver: Could not open file for writing!";
        return;
    }
}

void FileReceiver::onReadyRead()
{
    QByteArray data = tcpSocket->readAll();
    incomingFile.write(data);

    if (tcpSocket->state() == QAbstractSocket::UnconnectedState) {
        qDebug() << "Receiver: File transfer complete! Saved to" << incomingFile.fileName();
        incomingFile.close();
        deleteLater();
    }
}
