#include "view_connection_server.h"
#include "view_connection_trd.h"
#include "common.h"

ViewTrd::ViewTrd( quintptr soc )
{
     wrapper_ = new ViewTrdWrapper( soc );
     wrapper_->moveToThread( &trd_ );
     connect( &trd_, &QThread::started, wrapper_, &ViewTrdWrapper::onStart, Qt::QueuedConnection );
     connect( this, &ViewTrd::sStop, wrapper_, &ViewTrdWrapper::onStop, Qt::BlockingQueuedConnection );
     connect( wrapper_, &ViewTrdWrapper::sDisconnected, this, &ViewTrd::onDisconnect );
}

ViewTrd::~ViewTrd()
{
     onStop();
}


void ViewTrd::startTrd()
{
     trd_.start();
}


void ViewTrd::onDisconnect()
{
     // никаких доп. действий не требуются т.к. они отработают в деструкторе
     emit sDisconnected( this );
}


void ViewTrd::onStop()
{
     if( !wrapper_ )
     {
          return;
     }
     emit sStop( QPrivateSignal() );

     trd_.quit();
     if( trd_.isRunning() )
     {
          if( !trd_.wait( 30000 ) )
          {
               qCritical() << "Cannot correct stop ViewTrdWrapper";
               trd_.terminate();
          }
     }
     wrapper_->deleteLater();
     wrapper_ = nullptr;
}









#define DEBUG_VIEWTRD qDebug() << "View trd wrapper id" << QThread::currentThreadId()
#define CRITICAL_VIEWTRD qCritical() << "View trd wrapper id" << QThread::currentThreadId()


ViewTrdWrapper::ViewTrdWrapper( quintptr socket )
:sockHandle_( socket )
{
     connect( this, &ViewTrdWrapper::sViewRequest, this, &ViewTrdWrapper::onViewRequestDbg );
     /// todo: remove me

     connect( this, &ViewTrdWrapper::sViewRequest, this, &ViewTrdWrapper::onSendViewResponse );
}


ViewTrdWrapper::~ViewTrdWrapper()
{
     if( !socket_ )
     {
          return;
     }
     socket_->close();
     delete socket_;
     socket_ = nullptr;
}


void ViewTrdWrapper::onStart()
{
     DEBUG_VIEWTRD << " started";

     socket_ = new QTcpSocket;
     connect( socket_, &QTcpSocket::readyRead, this, &ViewTrdWrapper::onReadyRead );
     connect( socket_, &QTcpSocket::errorOccurred, this, &ViewTrdWrapper::onSocketError );

     if( !( socket_->setSocketDescriptor( sockHandle_ ) && socket_->open( QIODevice::ReadWrite ) ) )
     {
          CRITICAL_VIEWTRD << "Error setSD or open socket" << socket_->error();
          socket_->close();
          delete socket_;
          socket_ = nullptr;
     }
}


void ViewTrdWrapper::onStop()
{
     socket_->close();
     pendingData_.clear();
}


void ViewTrdWrapper::onReadyRead()
{
     pendingData_ += socket_->readAll();
     handleReceivedData();
     while( !requests_.isEmpty() )
     {
          emit sViewRequest( requests_.first() );
          requests_.removeFirst();
     }
}


void ViewTrdWrapper::handleReceivedData()
{
     while( true )
     {
          ViewCommand cmd = deserializeViewCmd( pendingData_ );
          if( !cmd.isValid() )
          {
               break;
          }
          requests_.push_back( cmd );
     }
}


void ViewTrdWrapper::onSocketError( QAbstractSocket::SocketError error )
{
     if( error == QAbstractSocket::RemoteHostClosedError )
     {
          DEBUG_VIEWTRD << "View trd wrapper: close connection";
     }
     else
     {
          CRITICAL_VIEWTRD << "View trd wrapper error: " << error;
     }
     emit sDisconnected();
}


void ViewTrdWrapper::onSendViewResponse( const ViewCommand& cmd )
{
     onSendViewResponseDbg( cmd );
     if( !socket_->write( serializeViewCmd( cmd ) ) )
     {
          CRITICAL_VIEWTRD << "Cannot send cmd" << socket_->error();
     }
}


void ViewTrdWrapper::onSendViewResponseDbg( const ViewCommand& cmd )
{
     DEBUG_VIEWTRD << "Send response" << cmd;
}


void ViewTrdWrapper::onViewRequestDbg( const ViewCommand& cmd )
{
     DEBUG_VIEWTRD << "Received request" << cmd;
}