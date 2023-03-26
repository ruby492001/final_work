#pragma once

#include <QtCore>
#include <QtNetwork>

#include "tcp_listen_server.h"
#include "view_connection_trd.h"


class ViewConnectionServer: public QObject
{
     Q_OBJECT
public:
     ViewConnectionServer( quint16 port );
     virtual ~ViewConnectionServer();
     bool startListening();

private slots:
     void onNewConnection( quintptr handle );
     void onDisconnect( ViewTrd* ptr );

private:
     TcpListenServer* server_;
     QList< ViewTrd* > trds_;
     quint16 port_;
};