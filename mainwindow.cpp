#include "mainwindow.h"
#include "./ui_mainwindow.h"

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
        ui->UserList->addItem(displayText);
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
