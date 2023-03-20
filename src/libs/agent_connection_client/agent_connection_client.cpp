#include "agent_connection_client.h"
#include "agent_proto.h"

AgentConnectionClient::AgentConnectionClient( const QString& ipAddress, const quint16 port )
:port_( port )
{
     connect( &sock_, &QTcpSocket::errorOccurred, this, &AgentConnectionClient::onSocketError );
     addr_.setAddress( ipAddress );
}


bool AgentConnectionClient::setAgentId( quint32 agentId, const QString& uuid )
{
     if( uuid.size() != UUID_SIZE || agentId == INVALID_AGENT_ID )
     {
          return false;
     }
     agentId_ = agentId;
     uuid_ = uuid;
     return true;
}


AgentConnectionClientStates AgentConnectionClient::connectedState()
{
     return state_;
}


void AgentConnectionClient::onConnect()
{
     if( addr_.isNull() )
     {
          qCritical() << "Address error";
          return;
     }
     sock_.connectToHost( addr_, port_ );
     if( !sock_.waitForConnected( 10000 ) )
     {
          state_ = AgentConnectionClientError;
          qCritical() << "Connection timeout";
          return;
     }
     state_ = AgentConnectionClientConnected;
     if( agentId_ != INVALID_AGENT_ID )
     {
          if( onNotifyServer() )
          {
               state_ = AgentConnectionClientActive;
          }
          else
          {
               state_ = AgentConnectionClientProtocolError;
          }
     }
     if( state_ != AgentConnectionClientActive && state_ != AgentConnectionClientProtocolError )
     {
          if( !onRequestAgentId() )
          {
               state_ = AgentConnectionClientProtocolError;
          }
          else
          {
               state_ = AgentConnectionClientActive;
          }
     }

     if( state_ == AgentConnectionClientProtocolError )
     {
          sock_.close();
     }
}


void AgentConnectionClient::onAddEvent( const AgentEvent& event )
{
     if( !event.isValid() || event.type() == AgentEventUndefined )
     {
          return;
     }
     queue_.push_back( event );
}


bool AgentConnectionClient::onFlushEvents()
{
     if( queue_.isEmpty() )
     {
          return true;
     }
     if( state_ != AgentConnectionClientActive )
     {
          return false;
     }
     bool status = sendEvents();
     if( status )
     {
          queue_.clear();
     }
     return status;
}


bool AgentConnectionClient::onNotifyServer()
{
     if( sock_.state() != QAbstractSocket::ConnectedState )
     {
          return false;
     }
     if( state_ == AgentConnectionClientActive )
     {
          return true;
     }

     AgentConnectionRequestAgentId req;
     req.req.cmd = LOGIN_REQUEST_CMD;
     req.agentId = agentId_;
     memcpy( req.uuid, uuid_.toStdString().c_str(), uuid_.size() );
     QByteArray pack = packData( reinterpret_cast< unsigned char* >( &req ), sizeof( req ) );
     if( sock_.write( pack ) != pack.size() )
     {
          qCritical() << "Error write to socket";
          return false;
     }
     if( !sock_.waitForBytesWritten( timeouts_ ) )
     {
          qCritical() << "Writing timeout";
          return false;
     }
     if( !sock_.waitForReadyRead( timeouts_ ) )
     {
          qCritical() << "Reading timeout";
          return false;
     }
     if( sock_.bytesAvailable() != sizeof( ServerProtoMain ) )
     {
          qCritical() << "Incorrect response size";
          return false;
     }
     QByteArray ba = sock_.readAll();
     ServerProtoMain* response =  reinterpret_cast< ServerProtoMain* >( ba.data() );
     if( response->cmd != LOGIN_REQUEST_RESPONSE_CMD )
     {
          qCritical() << "Incorrect response login cmd";
          return false;
     }
     if( !response->ok )
     {
          qCritical() << "Error on login:" << response->errorCode;
          return false;
     }
     state_ = AgentConnectionClientActive;
     return true;
}


bool AgentConnectionClient::onRequestAgentId()
{
     if( sock_.state() != QAbstractSocket::ConnectedState )
     {
          return false;
     }
     if( state_ == AgentConnectionClientActive )
     {
          return true;
     }
     uuid_ = generateUuid();
     AgentConnectionRequestAgentId req;
     req.req.cmd = GENERATE_AGENT_ID_REQUEST_CMD;
     memcpy( req.uuid, uuid_.toStdString().c_str(), uuid_.size() );
     QByteArray package = packData( reinterpret_cast< unsigned char* >( &req ), sizeof( req ) );
     if( sock_.write( package ) != package.size() )
     {
          qCritical() << "Error write to socket";
          return false;
     }

     if( !sock_.waitForBytesWritten( timeouts_ ) )
     {
          qCritical() << "Writing timeout";
          return false;
     }
     if( !sock_.waitForReadyRead( timeouts_ ) )
     {
          qCritical() << "Reading timeout";
          return false;
     }
     if( sock_.bytesAvailable() == sizeof( ServerProtoMain ) )
     {
          //произошла ошибка(скорее всего)
          QByteArray ba = sock_.readAll();
          ServerProtoMain* response =  reinterpret_cast< ServerProtoMain* >( ba.data() );
          if( response->ok )
          {
               qCritical() << "Incorrect response generate UUID";
          }
          else
          {
               qCritical() << "Error on login:" << response->errorCode;
          }
          return false;
     }
     else if( sock_.bytesAvailable() == sizeof( AgentConnectionResponse ) )
     {
          //ошибки нет(скорее всего)
          QByteArray ba = sock_.readAll();
          AgentConnectionResponse* response =  reinterpret_cast< AgentConnectionResponse* >( ba.data() );
          if( response->res.cmd != GENERATE_AGENT_ID_RESPONSE_CMD )
          {
               qCritical() << "Incorrect response login cmd";
               return false;
          }
          if( !response->res.ok )
          {
               qCritical() << "Error on login:" << response->res.errorCode;
               return false;
          }
          agentId_ = response->agentId;
          emit sGeneratedAgentId( agentId_, uuid_ );
          state_ = AgentConnectionClientActive;
          qDebug() << uuid_ << agentId_;
          return true;
     }
     qCritical() << "Incorrect response size";
     return false;
}


QByteArray AgentConnectionClient::prepareEvent( const AgentEvent& event )
{
     QByteArray res;
     int resSize;
     if( event.type() == AgentEventTypeBool )
     {
          resSize = sizeof( OneEventDataRequest ) - sizeof( OneEventDataRequest::data );
     }
     else
     {
          resSize = sizeof( OneEventDataRequest );
     }
     res.resize( resSize );
     memset( reinterpret_cast< OneEventDataRequest* >( res.data() ), 0, resSize );

     OneEventDataRequest* str = reinterpret_cast< OneEventDataRequest* >( res.data() );
     str->id = event.sensorId();
     str->flagsMsecs = event.msecs();

     AgentEventType type = event.type();
     switch( type )
     {
          case AgentEventTypeBool:
          {
               str->flagsMsecs |= 1 << 10;
               if( event.boolValue() )
               {
                    str->flagsMsecs |= 1 << 15;
               }
               break;
          }
          case AgentEventTypeFloat:
          {
               str->flagsMsecs |= 1 << 11;
               float tmp = event.floatValue();
               memcpy( str->data, reinterpret_cast< void* >( &tmp ), sizeof( tmp ) );
               break;
          }
          case AgentEventTypeUInt32:
          {
               str->flagsMsecs |= 1 << 12;
               quint32 tmp = event.uint32Value();
               memcpy( str->data, reinterpret_cast< void* >( &tmp ), sizeof( tmp ) );
               break;
          }
          case AgentEventTypeInt32:
          {
               str->flagsMsecs |= 1 << 13;
               qint32 tmp = event.int32Value();
               memcpy( str->data, reinterpret_cast< void* >( &tmp ), sizeof( tmp ) );
               break;
          }
          default:
               return {};
     }
     return res;
}


QString AgentConnectionClient::generateUuid()
{
     const QString possibleCharacters( "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789" );
     const int randomStringLength = UUID_SIZE;

     QString randomString;
     for( int i = 0; i < randomStringLength; i++ )
     {
          int index = QRandomGenerator::global()->generate() % possibleCharacters.size();
          QChar nextChar = possibleCharacters.at( index );
          randomString.append( nextChar );
     }
     return randomString;
}


QByteArray AgentConnectionClient::prepareSomeEvents( const QList<AgentEvent>& list )
{
     //   time     event
     QMap< quint64, QList< const AgentEvent* > > sorted;
     for( auto listIt = list.begin(); listIt != list.end(); listIt++ )
     {
          auto timeIt = sorted.find( listIt->time() );
          if( timeIt == sorted.end() )
          {
               timeIt = sorted.insert( listIt->time(), QList< const AgentEvent* >() );
          }
          timeIt.value().push_back( &(*listIt) );
     }
     QByteArray data;
     for( QMapIterator< quint64, QList< const AgentEvent* > > timeList( sorted ); timeList.hasNext();  )
     {
          timeList.next();
          QByteArray oneTimeData;
          foreach( auto oneElement, timeList.value() )
          {
               oneTimeData += prepareEvent( *oneElement );
          }
          QByteArray oneTimeMessage;
          oneTimeMessage.resize( sizeof( OneTimeRequest ) - sizeof( OneTimeRequest::data ) + oneTimeData.size() );
          OneTimeRequest* req = reinterpret_cast< OneTimeRequest* > ( oneTimeMessage.data() );
          req->time = timeList.key();
          req->count = timeList.value().count();
          memcpy( req->data, oneTimeData.data(), oneTimeData.size() );
          data += oneTimeMessage;
     }
     return data;
}


bool AgentConnectionClient::sendEvents()
{
     if( queue_.isEmpty() )
     {
          return true;
     }
     QByteArray data = prepareSomeEvents( queue_ );
     QByteArray dataToSend;
     dataToSend.resize( data.size() + sizeof( AgentSendDataRequest ) - sizeof( AgentSendDataRequest::data ) );

     AgentSendDataRequest* req = reinterpret_cast<AgentSendDataRequest*>( dataToSend.data() );
     req->req.cmd = SEND_DATA_REQUEST_CMD;
     memcpy( req->data, data.data(), data.size() );

     QByteArray package = packData( dataToSend );
     if( sock_.write( package ) != package.size() )
     {
          qCritical() << "Cannot send data events to server";
          return false;
     }
     if( !sock_.waitForBytesWritten( timeouts_ ) )
     {
          qCritical() << "Timeout in waiting bytes writen";
          return false;
     }
     if( !sock_.waitForReadyRead( timeouts_ ) )
     {
          qCritical() << "Timeout in server response";
          return false;
     }
     QByteArray serverResponse = sock_.readAll();
     if( serverResponse.size() != sizeof( ServerProtoMain ) )
     {
          qCritical() << "Incorrect server response in send data";
          return false;
     }
     ServerProtoMain* response = reinterpret_cast< ServerProtoMain* >( serverResponse.data() );
     if( response->cmd != SEND_DATA_RESPONSE_CMD )
     {
         qCritical() << "Incorrect return code send events:" << response->cmd;
          return false;
     }
     if( !response->ok )
     {
          qCritical() << "Server return status: " << response->errorCode;
          return false;
     }
     return true;
}


QByteArray AgentConnectionClient::packData( const QByteArray& data )
{
     quint32 size = data.size() + sizeof( NetworkPackage ) - sizeof( NetworkPackage::data );
     QByteArray res;
     res.resize( size );
     NetworkPackage* package = reinterpret_cast<NetworkPackage*>( res.data() );
     package->size = size;
     memcpy( package->data, data.data(), data.size() );
     return res;
}


QByteArray AgentConnectionClient::packData( const unsigned char* data, quint32 dataSize )
{
     quint32 packageSize = dataSize + sizeof( NetworkPackage ) - sizeof( NetworkPackage::data );
     QByteArray res;
     res.resize( packageSize );
     NetworkPackage* package = reinterpret_cast<NetworkPackage*>( res.data() );
     package->size = packageSize;
     memcpy( package->data, data, dataSize );
     return res;
}


void AgentConnectionClient::onStateChanged( QAbstractSocket::SocketState newState_ )
{
     if( newState_ == QAbstractSocket::SocketState::ConnectingState )
     {
          qDebug() << "Try to connect";
          return;
     }
     if( newState_ == QAbstractSocket::SocketState::ConnectedState )
     {
          qDebug() << "Connected";
          return;
     }
     if( newState_ == QAbstractSocket::SocketState::ClosingState )
     {
          emit sConnectionLost();
          qDebug() << "Closing";
          return;
     }
}


void AgentConnectionClient::onSocketError( QAbstractSocket::SocketError socketError )
{
     emit sConnectionLost();
     qDebug() << socketError;
}
