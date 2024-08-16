#include <QCoreApplication>
#include <QTcpSocket>
#include <QTextStream>
#include "communication.h"

class MyTcpClient : public QObject {
    Q_OBJECT
public:
    MyTcpClient(QObject *parent = nullptr) : QObject(parent) {
        socket = new QTcpSocket(this);
        socket->connectToHost("127.0.0.1", 1234); // 连接到服务器

        connect(socket, &QTcpSocket::connected, this, &MyTcpClient::onConnected);
        connect(socket, &QTcpSocket::disconnected, this, &MyTcpClient::onDisconnected);
        connect(socket, &QTcpSocket::readyRead, this, &MyTcpClient::onReadyRead);
    }

private slots:
    void onConnected() {
        QTextStream stream(stdin);
        QString input;

        while (true) {
            qDebug() << "Enter command (SEND:message or CLOSE):";
            input = stream.readLine();

            if (input.startsWith("SEND:") || input == "CLOSE") {
                socket->write((input + "\n").toUtf8());
                socket->flush();
                if (input == "CLOSE") break;
            } else {
                qDebug() << "Unknown command!";
            }
        }

        socket->disconnectFromHost();
    }

    void onDisconnected() {
        qDebug() << "Disconnected from server.";
        QCoreApplication::quit();
    }

private:
    QTcpSocket *socket;
};

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    MyTcpClient client;
    return a.exec();
}
