#include "udp_broadcast.h"
#include <QHostAddress>

UdpBroadcast::UdpBroadcast(QObject *parent)
    : QObject(parent), broadcastPort(45454) {
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::Any, broadcastPort);

    connect(udpSocket, &QUdpSocket::readyRead, this, &UdpBroadcast::processPendingDatagrams);
}

void UdpBroadcast::sendBroadcast(const QString &message) {
    udpSocket->writeDatagram(message.toUtf8(), QHostAddress::Broadcast, broadcastPort);
}

void UdpBroadcast::processPendingDatagrams() {
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray buffer;
        QHostAddress sender;
        quint16 senderPort;

        buffer.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);
        
        emit receivedMessage(QString::fromUtf8(buffer));
    }
}
