#ifndef UDP_BROADCAST_H
#define UDP_BROADCAST_H

#include <QUdpSocket>
#include <QObject>

class UdpBroadcast : public QObject {
    Q_OBJECT
public:
    UdpBroadcast(QObject *parent = nullptr);
    void sendBroadcast(const QString &message);
    
signals:
    void receivedMessage(const QString &message);

private slots:
    void processPendingDatagrams();

private:
    QUdpSocket *udpSocket;
    quint16 broadcastPort;
};

#endif // UDP_BROADCAST_H
