#include <QCoreApplication>
#include <QTcpSocket>
#include <QTextStream>
#include <QHostAddress>
#include <QDebug>
#include "communication.h"
#include "udp_broadcast.h"

class MyP2PClient : public QObject {
    Q_OBJECT
public:
    MyP2PClient(QObject *parent = nullptr) 
        : QObject(parent), tcpSocket(new QTcpSocket(this)), udpBroadcast(new UdpBroadcast(this)) {
        connect(udpBroadcast, &UdpBroadcast::receivedMessage, this, &MyP2PClient::onBroadcastReceived);
        
        tcpSocket->connectToHost("127.0.0.1", 1234); // 可以设置为动态选择的地址
        
        connect(tcpSocket, &QTcpSocket::connected, this, &MyP2PClient::onConnected);
        connect(tcpSocket, &QTcpSocket::disconnected, this, &MyP2PClient::onDisconnected);
        connect(tcpSocket, &QTcpSocket::readyRead, this, &MyP2PClient::onReadyRead);
    }

    void broadcastPresence() {
        udpBroadcast->sendBroadcast("P2P_CLIENT");
    }

private slots:
    void onBroadcastReceived(const QString &message) {
        if (message == "P2P_CLIENT") {
            // 假设我们会尝试与接收到的客户端进行连接
            // 这里可以选择性地实现基于接收到的地址和端口进行连接
            qDebug() << "Found another P2P client!";
            // 实际连接的实现可以更复杂，例如记录地址和端口
        }
    }

    void onConnected() {
        QTextStream stream(stdin);
        QString input;
        
        while (true) {
            qDebug() << "Enter command (SEND:message or CLOSE):";
            input = stream.readLine();

            if (input.startsWith("SEND:") || input == "CLOSE") {
                tcpSocket->write((input + "\n").toUtf8());
                tcpSocket->flush();
                if (input == "CLOSE") break;
            } else {
                qDebug() << "Unknown command!";
            }
        }

        tcpSocket->disconnectFromHost();
    }

    void onDisconnected() {
        qDebug() << "Disconnected from server.";
        QCoreApplication::quit();
    }

    void onReadyRead() {
        while (tcpSocket->canReadLine()) {
            QString line = tcpSocket->readLine().trimmed();
            Command command = parseCommand(line);
            qDebug() << "Received command:" << line;
            // 根据需要处理命令
        }
    }

private:
    QTcpSocket *tcpSocket;
    UdpBroadcast *udpBroadcast;
};

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    MyP2PClient client;
    client.broadcastPresence(); // 启动时广播自己作为 P2P 客户端
    return a.exec();
}

#include "moc_p2p_client.cpp" // 确保包含 moc 生成的文件
