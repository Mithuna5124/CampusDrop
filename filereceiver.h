#ifndef FILERECEIVER_H
#define FILERECEIVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>

class FileReceiver : public QObject
{
    Q_OBJECT
public:
    explicit FileReceiver(const QString &fileName, QObject *parent = nullptr);
    void startListening();

signals:

private slots:
    void onNewConnection();
    void onReadyRead();

private:
    QTcpServer *tcpServer = nullptr;
    QTcpSocket *tcpSocket = nullptr;
    QFile incomingFile;
};

#endif // FILERECEIVER_H
