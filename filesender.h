#ifndef FILESENDER_H
#define FILESENDER_H

#include <QObject>
#include <QTcpSocket>
#include <QFile>
#include <QHostAddress>

class FileSender : public QObject
{
    Q_OBJECT
public:
    explicit FileSender(const QString &filePath, const QHostAddress &address, quint16 port, QObject *parent = nullptr);

public slots:
    void startSending();

private slots:
    void onConnected();
    void onBytesWritten(qint64 bytes);

signals:

private:
    QTcpSocket *tcpSocket = nullptr;
    QFile fileToSend;
    QHostAddress hostAddress;
    quint16 port;
    qint64 totalBytes = 0;
    qint64 bytesWritten = 0;
};

#endif // FILESENDER_H
