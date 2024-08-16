#include <QTcpServer>
#include <QTcpSocket>
#include <QCoreApplication>
#include "communication.h"

class MyTcpServer : public QTcpServer {
    Q_OBJECT
public:
    MyTcpServer(QObject *parent = nullptr) : QTcpServer(parent) {
        connect(this, &MyTcpServer::newConnection, this, &MyTcpServer::handleNewConnection);
    }

private slots:
    void handleNewConnection() {
        QTcpSocket *socket = nextPendingConnection();
        connect(socket, &QTcpSocket::readyRead, this, [this, socket]() {
            while (socket->canReadLine()) {
                QString line = socket->readLine().trimmed();
                Command command = parseCommand(line);
                if (command.type == CommandType::SEND) {
                    qDebug() << "Received message:" << command.payload;
                } else if (command.type == CommandType::CLOSE) {
                    socket->close();
                    break;
                }
            }
        });
    }
};

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    MyTcpServer server;
    server.listen(QHostAddress::Any, 1234);
    qDebug() << "Server started on port 1234";
    return a.exec();
}
