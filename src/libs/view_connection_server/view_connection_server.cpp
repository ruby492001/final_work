#include "view_connection_server.h"

ViewConnectionServer::ViewConnectionServer( quint16 port )
:port_( port )
{
     server_ = new TcpListenServer;
     connect( server_, &TcpListenServer::sNewConnection, this, &ViewConnectionServer::onNewConnection );
}


ViewConnectionServer::~ViewConnectionServer()
{
     if( server_->isListening() )
     {
          server_->stopListening();
     }
     server_->deleteLater();
}


bool ViewConnectionServer::startListening()
{
     return server_->startListening( port_ );
}


void ViewConnectionServer::onNewConnection( quintptr handle )
{
     ViewTrd* connection = new ViewTrd( handle );
     connect( connection, &ViewTrd::sDisconnected, this, &ViewConnectionServer::onDisconnect, Qt::QueuedConnection );
     trds_.push_back( connection );
     connection->startTrd();
}


void ViewConnectionServer::onDisconnect( ViewTrd* ptr )
{
     for( auto it = trds_.begin(); it != trds_.end(); it++ )
     {
          if( *it == ptr )
          {
               ( *it )->deleteLater();
               trds_.erase( it );
               return;
          }
     }
     qCritical() << "ViewTrd not deleted";
}