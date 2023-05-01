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


AddToWriteEventResult EventWriter::addToWriteEvent( quint32 clientId, QList<std::shared_ptr<AgentEvent>>&& events )
{
     //LOG_DURATION( "AddToWriteEvent" )
     AddToWriteEventResult res = AtwerOk;

     // проверяем соответствие типов данных
//     if( !validateEventsTypes( clientId, events ) )
//     {
//          res = AtwerDataTypeError;
//     }

     QMutexLocker lock( &writeQueueMutex_ );

     if( ( long long )errorCount_ < events.count() + writeQueue_.count() )
     {
          res = AtwerCountError;
          qCritical() << "Аварийное состояние! Завершение всех соединений и остановка программы!";
          exit( -1 );
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
          QQueue< QPair< quint32,  QList< std::shared_ptr<AgentEvent> > > > internalQueue( std::move( writeQueue_ ) );
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

bool EventWriter::writeEvents( const QQueue< QPair< quint32, QList< std::shared_ptr<AgentEvent> > > >& events )
{
     SqlWrapperProtection sql( SqlConnectionManager::getWriteConnection() );
     sql->beginTransaction();
     DataToWrite dataToWrite;
     //LogDuration* prepare = new LogDuration( "Prepare" );
     foreach( const auto& eventsOneClient, events )
     {
          foreach( const auto& event, eventsOneClient.second )
          {
               if( !addTypeIfNeed( eventsOneClient.first, event->sensorId(), event->type(), &sql ) )
               {
                    return false;
               }
               prepareEvent( eventsOneClient.first, *event, dataToWrite );
          }
     }
     //delete prepare;

     //prepare = new LogDuration( "Write" );
     sql->writeSomeEvents( dataToWrite.clientId, dataToWrite.sensorId, dataToWrite.time, dataToWrite.msecs, dataToWrite.data );
     sql->commitTransaction();
     //delete prepare;
     return true;
}


bool EventWriter::isInited() const
{
     return inited_;
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


bool EventWriter::validateEventsTypes( quint32 clientId, const QList<std::shared_ptr<AgentEvent>>& events )
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


void EventWriter::prepareEvent( quint32 clientId, const AgentEvent& event, DataToWrite& data )
{
     QVariant time;
     if( event.msecsExist() )
     {
          time = event.msecs();
     }
     data.clientId.push_back( clientId );
     data.sensorId.push_back( event.sensorId() );
     data.time.push_back( event.time() );
     data.msecs.push_back( time );


     switch( event.type() )
     {
          case AgentEventTypeBool:
          {
               if( event.boolValue() )
               {
                    data.data.push_back( TO_QBYTEARRAY( cacheTrueVal ) );
               }
               else
               {
                    data.data.push_back( TO_QBYTEARRAY( cacheFalseVal ) );
               }
               break;
          }
          case AgentEventTypeInt32:
          {
               cacheQint = event.int32Value();
               data.data.push_back( TO_QBYTEARRAY( cacheQint ) );
               break;
          }
          case AgentEventTypeUInt32:
          {
               cacheQuint = event.uint32Value();
               data.data.push_back( TO_QBYTEARRAY( cacheQuint ) );
               break;
          }
          case AgentEventTypeFloat:
          {
               cacheFloat = event.floatValue();
               data.data.push_back( TO_QBYTEARRAY( cacheFloat ) );
               break;
          }
          default:
          {
               return;
          }
     }
}

quint64 EventWriter::currentQueueCount()
{
     quint64 count = 0;
     QMutexLocker lock( &writeQueueMutex_ );
     for( int a = 0; a < writeQueue_.count(); a++ )
     {
          count += writeQueue_.at( a ).second.count();
     }
     return count;
}
