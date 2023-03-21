#include "tcp_listen_server.h"


TcpListenServer::TcpListenServer()
        :QTcpServer()
{
}


bool TcpListenServer::startListening( quint16 port )
{
     return listen( QHostAddress::AnyIPv4, port );
}


void TcpListenServer::stopListening()
{
     if( isListening() )
     {
          stopListening();
     }
}


void TcpListenServer::incomingConnection( qintptr handle )
{
     emit sNewConnection( handle );
}