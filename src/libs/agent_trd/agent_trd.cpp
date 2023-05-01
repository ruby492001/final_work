#include "agent_trd.h"
#include "agent_proto.h"
#include "common.h"
#include "cred_validator.h"
#include "event_writer.h"
#include "profile.h"


ClientTrd::ClientTrd( quintptr socket )
{
     wrapper_ = new ClientTrdWrapper( this );
     wrapper_->moveToThread( &trd_ );
     connect( this, &ClientTrd::sStop, wrapper_, &ClientTrdWrapper::onStop, Qt::BlockingQueuedConnection );

     connect( this, &ClientTrd::sSetSocketDescriptor, wrapper_, &ClientTrdWrapper::onSetSocketDesc,
              static_cast< Qt::ConnectionType >( Qt::QueuedConnection | Qt::SingleShotConnection ) );
     emit sSetSocketDescriptor( socket );
}


ClientTrd::~ClientTrd()
{
     emit sStop( QPrivateSignal() );
     if( trd_.isRunning() )
     {
          trd_.exit();
          if( !trd_.wait( ) )
          {
               trd_.terminate();
          }
     }
     delete wrapper_;
}


ClientTrdWrapper* ClientTrd::getWrapper()
{
     return wrapper_;
}


void ClientTrd::startTrd()
{
     trd_.start();
}


ClientTrdWrapper::ClientTrdWrapper( QObject* par )
:QObject( nullptr ), par_( par )
{
}


void ClientTrdWrapper::onReadyRead()
{
     pendingData_ += socket_->readAll();
     handleData();
     if( !requests_.isEmpty() )
     {
          handleRequests();
     }
}


void ClientTrdWrapper::handleData()
{
     qsizetype packMinSize = sizeof( NetworkPackage );
     while( pendingData_.size() )
     {
          if( pendingData_.size() < packMinSize )
          {
               // не пришёл даже заголовок
               break;
          }

          NetworkPackage* package = reinterpret_cast< NetworkPackage* >( pendingData_.data() );

          if( package->size > pendingData_.size() )
          {
               // не все данные пришли
               break;
          }
          QByteArray packageData( reinterpret_cast< const char* >( package->data ), package->size - sizeof( NetworkPackage::size ) );
          pendingData_.remove( 0, package->size );
          package = nullptr;
          requests_.push_back( std::move( packageData ) );
     }

}


void ClientTrdWrapper::handleRequests()
{
     for( auto& current: requests_ )
     {
          if( current.size() < ( long )sizeof( AgentProtoMain ) )
          {
               continue;
          }
          AgentProtoMain* main = reinterpret_cast< AgentProtoMain* >( current.data() );
          switch( main->cmd )
          {
               case LOGIN_REQUEST_CMD:
               {
                    handleLoginRequest( current );
                    break;
               }
               case GENERATE_AGENT_ID_REQUEST_CMD:
               {
                    handleGenAgentIdRequest( current );
                    break;
               }
               case SEND_DATA_REQUEST_CMD:
               {
                    handleSendDataRequest( current );
                    break;
               }
               default:
               {
                    handleIncorrectRequest();
               }
          }
     }
     requests_.clear();
}


void ClientTrdWrapper::disconnect()
{
     onStop();
     if( par_ )
     {
          emit sDisconnected( qobject_cast< ClientTrd* >( par_ ) );
          par_ = nullptr;
     }
}


ClientTrdWrapper::~ClientTrdWrapper()
{
     onStop();
     delete socket_;
}


void ClientTrdWrapper::onStop()
{
     if( socket_->isOpen() )
     {
          socket_->close();
     }
}


void ClientTrdWrapper::handleLoginRequest( const QByteArray& arr )
{
     if( authorized_ )
     {
          returnError( INCORRECT_REQUEST_ERROR );
          return;
     }

     if( arr.size() != sizeof( AgentConnectionRequestAgentId ) )
     {
          returnError( INCORRECT_REQUEST_ERROR );
          return;
     }
     const AgentConnectionRequestAgentId* req = reinterpret_cast< const AgentConnectionRequestAgentId* >( arr.data() );


     ServerProtoMain res;
     res.cmd = LOGIN_REQUEST_RESPONSE_CMD;
     bool success = false;
     if( !CredValidator::validateCreds( req->agentId, QByteArray( req->uuid, sizeof( req->uuid ) ), success ) )
     {
          res.errorCode = INTERNAL_ERROR;
          res.ok = false;
     }
     else if( !success )
     {
          res.errorCode = INCORRECT_LOGIN_ERROR;
          res.ok = false;
     }
     else
     {
          res.cmd = LOGIN_REQUEST_RESPONSE_CMD;
          res.errorCode = NO_ERROR;
          res.ok = true;
          clientId_ = req->agentId;
     }
     qDebug() << "Login req" << req->agentId << QByteArray( req->uuid, sizeof( req->uuid ) ) << res.ok << res.errorCode;

     if( sizeof( res ) != socket_->write( reinterpret_cast< char* >( &res ), sizeof( res ) ) )
     {
          qCritical() << "Cannot write response for login request";
          disconnect();
     }
     if( !socket_->waitForBytesWritten( TIMEOUT ) )
     {
          qCritical() << "Timeout write response for login request";
          disconnect();
     }
     authorized_ = true;
}


void ClientTrdWrapper::returnError( quint32 errorCode )
{
     QByteArray response;
     response.reserve( sizeof( ServerProtoMain ) );
     ServerProtoMain* res = reinterpret_cast< ServerProtoMain* >( response.data() );
     res->ok = false;
     res->cmd = 0;
     res->errorCode = errorCode;
     if( socket_->write( response ) != response.size() )
     {
          qCritical() << "Write error error";
     }
     else
     {
          if( !socket_->waitForBytesWritten( TIMEOUT ) )
          {
               qCritical() << "Error waiting error written";
          }
     }
     disconnect();
}


void ClientTrdWrapper::handleGenAgentIdRequest( const QByteArray& arr )
{
     if( arr.size() != sizeof( AgentConnectionRequestAgentId ) )
     {
          returnError( INCORRECT_REQUEST_ERROR );
          return;
     }

     if( authorized_ )
     {
          returnError( INCORRECT_REQUEST_ERROR );
          return;
     }

     const AgentConnectionRequestAgentId* req = reinterpret_cast< const AgentConnectionRequestAgentId* >( arr.data() );

     AgentConnectionResponse res;
     res.res.cmd = GENERATE_AGENT_ID_RESPONSE_CMD;

     quint32 id;
     if( !CredValidator::createNewClient( QByteArray( req->uuid, sizeof( req->uuid ) ), id ) )
     {
          res.res.ok = false;
          res.res.errorCode = INTERNAL_ERROR;
     }
     else
     {
          res.res.ok = true;
          res.res.errorCode = NO_ERROR;
          clientId_ = res.agentId = id;
     }

     qDebug() << "Gen agent id req: res:" << res.res.ok << "error:" << res.res.errorCode << res.agentId;
     if( sizeof( res ) != socket_->write( reinterpret_cast< char* >( &res ), sizeof( res ) ) )
     {
          qCritical() << "Cannot write response for login request";
          disconnect();
     }
     if( !socket_->waitForBytesWritten( TIMEOUT ) )
     {
          qCritical() << "Timeout write response for login request";
          disconnect();
     }
     authorized_ = true;
}


void ClientTrdWrapper::handleSendDataRequest( const QByteArray& arr )
{
     const quint32 headerSize = sizeof( AgentSendDataRequest ) - sizeof( AgentSendDataRequest::data );
     if( !authorized_ )
     {
          returnError( INCORRECT_REQUEST_ERROR );
          return;
     }
     QByteArray dataOnly( arr.data() + headerSize, arr.size() - headerSize );
     QList< std::shared_ptr<AgentEvent> > events;
     if( !parseEvents( dataOnly, events ) )
     {
          events.clear();
          returnError( INCORRECT_REQUEST_ERROR );
          return;
     }

     QByteArray responseBa;
     responseBa.resize( sizeof( ServerProtoMain ) );
     ServerProtoMain* response = reinterpret_cast< ServerProtoMain* >( responseBa.data() );
     response->cmd = SEND_DATA_RESPONSE_CMD;
     switch( eventWriter->addToWriteEvent( clientId_, std::move( events ) ) )
     {
          case AtwerOk:
          {
               response->ok = true;
               response->errorCode = NO_ERROR;
               break;
          }
          case AtwerCountWarning:
          {
               response->ok = true;
               response->errorCode = TO_MANY_REQUEST_WARNING;
               break;
          }
          case AtwerCountError:
          {
               response->ok = false;
               returnError( TO_MANY_REQUESTS_ERROR );
               return;
          }
          case AtwerDataTypeError:
          {
               response->ok = false;
               response->errorCode = INCORRECT_DATA_TYPE_ERROR;
               break;
          }
     }
     //qDebug() << "Write data result: " << response->ok << response->errorCode;
     if( responseBa.size() != socket_->write( responseBa ) )
     {
          qCritical() << "Cannot write response for handle send data req";
          disconnect();
     }
     if( !socket_->waitForBytesWritten( TIMEOUT ) )
     {
          qCritical() << "Timeout write response for handle send data req";
          disconnect();
     }
}


void ClientTrdWrapper::handleIncorrectRequest()
{
     qCritical() <<  "Incorrect request catch";
     returnError( INCORRECT_REQUEST_ERROR );
}


void ClientTrdWrapper::onSocketError( QAbstractSocket::SocketError error )
{
     if( error == QAbstractSocket::SocketError::RemoteHostClosedError )
     {
          // соединение было закрыто агентом, корректно завершаем работу
          qDebug() << "Connected close by agent";
     }
     else
     {
          qCritical() << "Socket error";
     }
     disconnect();
}


bool ClientTrdWrapper::parseEvents( const QByteArray& arr, QList< std::shared_ptr<AgentEvent> >& res )
{
     //LOG_DURATION( "Parse events" )
     if( ( unsigned long long )arr.size() < sizeof( OneTimeRequest ) )
     {
          return true;
     }
     const OneTimeRequest* oneTimeRequest = nullptr;
     const char* currentPtr = arr.data();
     quint64 time;
     const void* endPtr = arr.data() + arr.size();          // первый элемент за концом массива
     while( true )
     {
          oneTimeRequest = reinterpret_cast< const OneTimeRequest* >( currentPtr );
          currentPtr += sizeof( OneTimeRequest ) - sizeof( OneTimeRequest::data );
          if( currentPtr >= endPtr )
          {
               return false;
          }

          time = oneTimeRequest->time;

          // парсим события одного времени
          for( quint32 currentEvent = 0; currentEvent < oneTimeRequest->count; currentEvent++ )
          {
               const OneEventDataRequest* event = reinterpret_cast< const OneEventDataRequest* >( currentPtr );
               if( FLAG_ON( BOOL_TYPE, event->flagsMsecs ) )
               {
                    currentPtr += sizeof( OneEventDataRequest ) - sizeof( OneEventDataRequest::data );
               }
               else
               {
                    currentPtr += sizeof( OneEventDataRequest );
               }

               if( currentPtr > endPtr )
               {
                    return false;
               }
               if( FLAG_ON( event->flagsMsecs, BOOL_TYPE ) )
               {
                    res.push_back( std::make_shared< AgentEvent >( AgentEvent( event->id, time, MSEC( event->flagsMsecs ), BOOL_VALUE( event->flagsMsecs ) ) ) );
               }
               else if( FLAG_ON( event->flagsMsecs, FLOAT_TYPE ) )
               {
                    res.push_back( std::make_shared< AgentEvent >( AgentEvent( event->id, time, MSEC( event->flagsMsecs ), AgentEventTypeFloat, event->data ) ) );
               }
               else if( FLAG_ON( event->flagsMsecs, UIN32_TYPE ) )
               {
                    res.push_back( std::make_shared< AgentEvent >( AgentEvent( event->id, time, MSEC( event->flagsMsecs ), AgentEventTypeUInt32, event->data ) ) );
               }
               else if( FLAG_ON( event->flagsMsecs, INT32_TYPE ) )
               {
                    res.push_back( std::make_shared< AgentEvent >(  AgentEvent( event->id, time, MSEC( event->flagsMsecs ), AgentEventTypeInt32, event->data ) ) );
               }
               else
               {
                    qCritical() << "Incorrect data size";
                    return false;
               }
               if( !res.last()->isValid() )
               {
                    qCritical() << "Invalid event";
                    return false;
               }
          }
          if( currentPtr == endPtr )
          {
               return true;
          }
          if( currentPtr > endPtr )
          {
               return false;
          }
     }
}


void ClientTrdWrapper::onSetSocketDesc( quintptr nativeHandle )
{
     socket_ = new QTcpSocket( this );
     connect( socket_, &QTcpSocket::readyRead, this, &ClientTrdWrapper::onReadyRead, Qt::DirectConnection );
     connect( socket_, &QTcpSocket::errorOccurred, this, &ClientTrdWrapper::onSocketError, Qt::DirectConnection );
     if( !socket_->setSocketDescriptor( nativeHandle ) )
     {
          qCritical() << "Cannot set socket desc";
          disconnect();
     }
     if( !socket_->open( QIODevice::ReadWrite ) )
     {
          qCritical() << "Cannot open socket";
          disconnect();
     }
}