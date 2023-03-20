#include "event_writer.h"
#include "common.h"
#include "profile.h"


EventWriter* eventWriter = nullptr;


EventWriter::EventWriter( quint64 warnCount, quint64 errorCount )
:warnCount_( warnCount ), errorCount_( errorCount )
{
     SqlWrapperProtection sql( SqlConnectionManager::getReadOnlyConnection() );
     auto registeredTypes = sql->registeredDataTypes();
     foreach( const auto& registeredType, registeredTypes )
     {
          /// todo: сделать нормальный конвертер
          existTypes_.insert( registeredType.first, ( AgentEventType )registeredType.second );
     }

     qDebug() << "Event writer inited. Registered types count: " << existTypes_.count();
     inited_ = true;
     start( QThread::TimeCriticalPriority );
}


AddToWriteEventResult EventWriter::addToWriteEvent( quint32 clientId, QList<QPointer<AgentEvent>>&& events )
{
     LOG_DURATION( "AddToWriteEvent" )
     AddToWriteEventResult res = AtwerOk;

     // проверяем соответствие типов данных
     if( !validateEventsTypes( clientId, events ) )
     {
          res = AtwerDataTypeError;
     }

     QMutexLocker lock( &writeQueueMutex_ );

     if( ( long long )errorCount_ < events.count() + writeQueue_.count() )
     {
          res = AtwerCountError;
     }

     if( res == AtwerOk )
     {
          if( ( long long )warnCount_ < events.count() + writeQueue_.count() )
          {
               res = AtwerCountWarning;
          }
          writeQueue_.push_back( qMakePair( clientId, std::move( events ) ) );
          queueCondition_.wakeOne();
     }

     return res;
}


void EventWriter::run()
{
     while( true )
     {
          QMutexLocker lock( &writeQueueMutex_ );
          if( writeQueue_.isEmpty() )
          {
               if( !work_ )
               {
                    break;
               }
               queueCondition_.wait( &writeQueueMutex_ );
               lock.unlock();
               continue;
          }
          QQueue< QPair< quint32,  QList< QPointer< AgentEvent > > > > internalQueue( std::move( writeQueue_ ) );
          writeQueue_.clear();
          lock.unlock();
          if( !writeEvents( internalQueue ) )
          {
               lock.relock();
               std::move( internalQueue.constBegin(), internalQueue.constEnd(), std::front_inserter( writeQueue_ ) );
          }
     }
}

void EventWriter::stop()
{
     work_ = false;
     QMutexLocker lock( &writeQueueMutex_ );
     queueCondition_.wakeOne();
     lock.unlock();
     if( isRunning() )
     {
          if( !wait( 30000 ) )
          {
               qCritical() << "Cannot correct stop EventWriterThread";
               terminate();
          }
     }
}

bool EventWriter::writeEvents( const QQueue< QPair< quint32, QList< QPointer< AgentEvent > > > >& events )
{
     LOG_DURATION( QString( "WriteEvents: " + QString::number( events.first().second.count() ) ).toStdString().c_str() )
     SqlWrapperProtection sql( SqlConnectionManager::getWriteConnection() );

     sql->beginTransaction();
     foreach( const auto& eventsOneClient, events )
     {
          foreach( const auto& event, eventsOneClient.second )
          {
               if( !addTypeIfNeed( eventsOneClient.first, event->sensorId(), event->type(), &sql ) )
               {
                    return false;
               }
               if( !writeOneEvent( eventsOneClient.first, *event, &sql ) )
               {
                    return false;
               }
          }
     }
     sql->commitTransaction();
     return true;
}


bool EventWriter::isInited() const
{
     return inited_;
}


bool EventWriter::writeOneEvent( quint32 clientId, const AgentEvent& event, SqlWrapperProtection* sqlConnection )
{
     QVariant time;
     if( event.msecsExist() )
     {
          time = event.msecs();
     }
     switch( event.type() )
     {
          case AgentEventTypeBool:
          {
               if( event.boolValue() )
               {
                    return ( *sqlConnection )->writeEvent( clientId, event.sensorId(), event.time(), time, TO_QBYTEARRAY( cacheTrueVal ) );
               }
               else
               {
                    return ( *sqlConnection )->writeEvent( clientId, event.sensorId(), event.time(), time, TO_QBYTEARRAY( cacheFalseVal ) );
               }
          }
          case AgentEventTypeInt32:
          {
               cacheQint = event.int32Value();
               return ( *sqlConnection )->writeEvent( clientId, event.sensorId(), event.time(), time, TO_QBYTEARRAY( cacheQint ) );
          }
          case AgentEventTypeUInt32:
          {
               cacheQuint = event.uint32Value();
               return ( *sqlConnection )->writeEvent( clientId, event.sensorId(), event.time(), time,
                                                    TO_QBYTEARRAY( cacheQuint ) );
          }
          case AgentEventTypeFloat:
          {
               cacheFloat = event.floatValue();
               return ( *sqlConnection )->writeEvent( clientId, event.sensorId(), event.time(), time,
                                                    TO_QBYTEARRAY( cacheFloat ) );
          }
          default:
          {
               return false;
          }
     }
}


bool EventWriter::addTypeIfNeed( quint32 clientId, quint32 sensorId, const AgentEventType& type,
                                 SqlWrapperProtection* sqlConnection )
{
     quint64 clientAndSensorId = clientId;
     clientAndSensorId <<= 32;
     clientAndSensorId |= sensorId;

     QReadLocker readLock( &existTypesMutex_ );
     if( !existTypes_.contains( clientAndSensorId ) )
     {
          readLock.unlock();
          QWriteLocker writeLock( &existTypesMutex_ );
          existTypes_.insert( clientAndSensorId, type );
          writeLock.unlock();

          /// todo: сделать нормально преобразование
          return ( *sqlConnection )->registerNewDataType( clientAndSensorId, ( quint8 )type );
     }
     return true;
}


bool EventWriter::validateEventsTypes( quint32 clientId, const QList<QPointer<AgentEvent>>& events )
{
     QReadLocker lock( &existTypesMutex_ );
     foreach( const auto& event, events )
     {
          if( !validateTypeIfExist( clientId, event->sensorId(), event->type() ) )
          {
               return false;
          }
     }
     return true;
}


bool EventWriter::validateTypeIfExist( quint32 clientId, quint32 sensorId, const AgentEventType& type )
{
     quint64 clientAndSensorId = clientId;
     clientAndSensorId <<= 32;
     clientAndSensorId |= sensorId;

     auto element = existTypes_.find( clientAndSensorId );
     if( element == existTypes_.end() )
     {
          return true;
     }
     if( element.value() != type )
     {
          return false;
     }
     return true;
}
