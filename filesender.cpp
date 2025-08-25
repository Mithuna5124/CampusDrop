#include "filesender.h"
#include <QDebug>

FileSender::FileSender(const QString &filePath, const QHostAddress &address, quint16 port, QObject *parent)
    : QObject{parent}, hostAddress(address), port(port)
{
    fileToSend.setFileName(filePath);
    if (!fileToSend.open(QIODevice::ReadOnly)) {
        qDebug() << "Sender: Could not open file for reading!";
        return;
    }
    totalBytes = fileToSend.size();
    bytesWritten = 0;

    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::connected, this, &FileSender::onConnected);
    connect(tcpSocket, &QTcpSocket::bytesWritten, this, &FileSender::onBytesWritten);
}

void FileSender::startSending()
{
    qDebug() << "Sender: Trying to connect to" << hostAddress.toString() << "on port" << port;
    tcpSocket->connectToHost(hostAddress, port);
}

void FileSender::onConnected()
{
    qDebug() << "Sender: Connected to host. Starting transfer...";
    onBytesWritten(0);
}

void FileSender::onBytesWritten(qint64 bytes)
{
    bytesWritten += bytes;

    if (bytesWritten == totalBytes) {
        qDebug() << "Sender: File transfer complete!";
        tcpSocket->close();
        fileToSend.close();
        deleteLater();
        return;
    }

    if (!fileToSend.atEnd()) {
        QByteArray buffer = fileToSend.read(4096);
        tcpSocket->write(buffer);
    }
}
