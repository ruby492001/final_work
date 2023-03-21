#include "agent_connection_server.h"


AgentConnectionServer::AgentConnectionServer( quint16 port )
:server_( new TcpListenServer ), port_( port )
{
     server_ = new TcpListenServer;
     connect( server_, &TcpListenServer::sNewConnection, this, &AgentConnectionServer::onNewConnection );
}


AgentConnectionServer::~AgentConnectionServer()
{
     server_->close();
     server_->deleteLater();
     foreach( auto cur, trds )
     {
          delete cur;
     }
     trds.clear();
}


void AgentConnectionServer::onNewConnection( quintptr handle )
{
     ClientTrd* newTrd = new ClientTrd( handle );
     ClientTrdWrapper* wrapper = newTrd->getWrapper();

     connect( wrapper, SIGNAL( sDisconnected( ClientTrd * ) ), this,
              SLOT( onDisconnected( ClientTrd * ) ), Qt::QueuedConnection );

     trds.push_back( newTrd );

     newTrd->startTrd();
}


void AgentConnectionServer::onDisconnected( ClientTrd* trd )
{
     for( auto current = trds.begin(); current != trds.end(); current++ )
     {
          if( *current == trd )
          {
               ( *current )->deleteLater();
               trds.erase( current );
               return;
          }
     }
     qWarning() << "Not deleted trd";
}


bool AgentConnectionServer::startListening()
{
     return server_->startListening( port_ );
}
