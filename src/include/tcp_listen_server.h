#pragma once


#include <QtCore>
#include <QtNetwork>


class TcpListenServer: public QTcpServer
{
     Q_OBJECT
public:
     TcpListenServer();
     bool startListening( quint16 port );
     void stopListening();

signals:
     void sNewConnection( quintptr handle );

protected:
     void incomingConnection( qintptr handle ) override;
};