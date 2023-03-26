#include "view_connection_client.h"
#include "view_cmd.h"


ViewConnectionClient::ViewConnectionClient( const QString& address, quint16 port )
:QObject( nullptr ), sock_( new QTcpSocket ), addr_( address ), port_( port )
{
     connect( sock_, &QTcpSocket::readyRead, this, &ViewConnectionClient::onDataAvailable );
     connect( sock_, &QTcpSocket::errorOccurred, this, &ViewConnectionClient::onConnectionLost );
}

ViewConnectionClient::~ViewConnectionClient()
{
     if( sock_->state() == QTcpSocket::ConnectedState )
     {
          sock_->disconnectFromHost();
          sock_->close();
     }
     sock_->deleteLater();
     sock_ = nullptr;
}


void ViewConnectionClient::onConnect()
{
     // для кейса с повторным подключением
     onStop();
     sock_->connectToHost( addr_, port_ );
     // errorOccurred будет послан в случае ошибки
     if( sock_->waitForConnected( 30000 ) && sock_->state() == QTcpSocket::ConnectedState )
     {
          emit sConnected();
     }
}


void ViewConnectionClient::onStop()
{
     if( sock_->state() == QTcpSocket::ConnectedState )
     {
          sock_->disconnectFromHost();
          sock_->close();
     }
}


void ViewConnectionClient::onSendCommand( const ViewCommand& cmd )
{
     if( sock_->state() != QTcpSocket::ConnectedState )
     {
          return;
     }
     if( !sock_->write( serializeViewCmd( cmd ) ) )
     {
          qCritical() << "Cannot write cmd";
     }
}


void ViewConnectionClient::onDataAvailable()
{
     pendingData_ += sock_->readAll();
     while( true )
     {
          ViewCommand cmd = deserializeViewCmd( pendingData_ );
          if( cmd.isValid() )
          {
               emit sResponseReceived( cmd );
          }
          else
          {
               break;
          }
     }
}


void ViewConnectionClient::onConnectionLost()
{
     //qCritical() << "Connection error";
     emit sConnectionError();
}
