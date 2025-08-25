#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include "filereceiver.h"
#include "filesender.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    networkManager = new NetworkManager(this);
    networkManager->initSocket();
    connect(networkManager, &NetworkManager::newUserFound, this, &MainWindow::onNewUserFound);

    timeoutTimer = new QTimer(this);
    connect(timeoutTimer, &QTimer::timeout, this, &MainWindow::checkTimeouts);
    timeoutTimer->start(1000);

    connect(ui->UserList, &QListWidget::itemDoubleClicked, this, &MainWindow::onUserDoubleClicked);

    connect(networkManager, &NetworkManager::fileRequestReceived, this, &MainWindow::onFileRequestReceived);
    connect(networkManager, &NetworkManager::transferAccepted, this, &MainWindow::onTransferAccepted);
    connect(networkManager, &NetworkManager::transferDeclined, this, &MainWindow::onTransferDeclined);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onNewUserFound(quint64 id, const QString &ipAddress)
{
    (void)ipAddress;
    QString displayText = "User_" + QString::number(id);
    if (!onlineUsers.contains(id)) {
        QListWidgetItem *newItem = new QListWidgetItem(displayText);
        newItem->setData(Qt::UserRole, QVariant::fromValue(id));
        ui->UserList->addItem(newItem);
    }
    onlineUsers[id] = {displayText, QDateTime::currentDateTime()};
}

void MainWindow::checkTimeouts()
{
    const qint64 timeoutSeconds = 15;
    QDateTime currentTime = QDateTime::currentDateTime();

    for (auto it = onlineUsers.begin(); it != onlineUsers.end(); ) {
        if (it.value().lastSeen.secsTo(currentTime) > timeoutSeconds) {
            QString displayText = it.value().displayText;
            QList<QListWidgetItem*> items = ui->UserList->findItems(displayText, Qt::MatchExactly);
            if (!items.isEmpty()) {
                delete items.at(0);
            }
            it = onlineUsers.erase(it);
        } else {
            ++it;
        }
    }
}

void MainWindow::onUserDoubleClicked(QListWidgetItem *item)
{
    qDebug() << "Double-clicked on user:" << item->text();

    QString filePath = QFileDialog::getOpenFileName(this, "Choose a file to send");
    if (filePath.isEmpty()) {
        return;
    }
    this->lastFileToSendPath = filePath;

    quint64 targetId = item->data(Qt::UserRole).toULongLong();
    networkManager->sendFileRequest(targetId, filePath);
}

void MainWindow::onFileRequestReceived(quint64 senderId, const QString &fileName, qint64 fileSize)
{
    lastSenderId = senderId;

    QString senderDisplayText = "User_" + QString::number(senderId);
    if (onlineUsers.contains(senderId)) {
        senderDisplayText = onlineUsers[senderId].displayText;
    }

    QString fileSizeStr;
    if (fileSize > 1024 * 1024) {
        fileSizeStr = QString::number(fileSize / (1024.0 * 1024.0), 'f', 2) + " MB";
    } else if (fileSize > 1024) {
        fileSizeStr = QString::number(fileSize / 1024.0, 'f', 2) + " KB";
    } else {
        fileSizeStr = QString::number(fileSize) + " Bytes";
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Incoming File Transfer",
                                  QString("%1 wants to send you the file:\n\n%2 (%3)\n\nDo you accept?")
                                      .arg(senderDisplayText, fileName, fileSizeStr),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        networkManager->sendFileReply(lastSenderId, true);
        FileReceiver *receiver = new FileReceiver(fileName, this);
        receiver->startListening();
    } else {
        networkManager->sendFileReply(lastSenderId, false);
    }
}

void MainWindow::onTransferAccepted(quint64 senderId, const QHostAddress &senderIp)
{
    QString senderDisplayText = "User_" + QString::number(senderId);
    if (onlineUsers.contains(senderId)) {
        senderDisplayText = onlineUsers[senderId].displayText;
    }
    QMessageBox::information(this, "Transfer Accepted",
                             QString("%1 has accepted your file transfer request.")
                                 .arg(senderDisplayText));

    FileSender *sender = new FileSender(lastFileToSendPath, senderIp, 54545, this);
    sender->startSending();
}

void MainWindow::onTransferDeclined(quint64 senderId)
{
    QString senderDisplayText = "User_" + QString::number(senderId);
    if (onlineUsers.contains(senderId)) {
        senderDisplayText = onlineUsers[senderId].displayText;
    }
    QMessageBox::information(this, "Transfer Declined",
                             QString("%1 has declined your file transfer request.")
                                 .arg(senderDisplayText));
}
