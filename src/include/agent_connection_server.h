#pragma once


#include <QtCore>
#include <QtNetwork>

#include "agent_trd.h"

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


class AgentConnectionServer: public QObject
{
     Q_OBJECT
public:
     AgentConnectionServer( quint16 port );
     ~AgentConnectionServer();
     bool startListening();
private slots:
     void onNewConnection( quintptr handle );
     void onDisconnected( ClientTrd* trd );
private:
     TcpListenServer* server_;
     QList<ClientTrd*> trds;
     quint16 port_;
};