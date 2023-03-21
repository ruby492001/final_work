#pragma once


#include <QtCore>
#include <QtNetwork>

#include "agent_trd.h"
#include "tcp_listen_server.h"


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