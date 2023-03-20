#include <QSqlError>
#include "db_wrapper.h"

SqlWrapper* SqlConnectionManager::writeConnection_ = nullptr;
bool SqlConnectionManager::writeConnectionIsFree_ = true;
QMutex SqlConnectionManager::mutex_;
QWaitCondition SqlConnectionManager::condWrite_;
QList< SqlWrapper* > SqlConnectionManager::activeReadConnection_;
QList< SqlWrapper* > SqlConnectionManager::notActiveReadConnection_;
quint32 SqlConnectionManager::criticalWriteRequestCount_ = 0;
bool SqlConnectionManager::inited_ = false;
quint32 SqlConnectionManager::connectionNumber_ = 0;
QString SqlConnectionManager::pathToDb_;


SqlConnectionManager::SqlConnectionManager( const QString& pathToDb )
{
     QMutexLocker lock( &mutex_ );
     if( inited_ )
     {
          qCritical() << "Sql connection manager already inited";
          return;
     }
     pathToDb_ = pathToDb;
     writeConnection_ = new SqlWrapper( pathToDb, connectionNumber_++, false );
     if( !writeConnection_->initDb() )
     {
          qCritical() << "Cannot init db schema";
          return;
     }
     inited_ = true;
     qDebug() << "Sql connection manager inited";
}


bool SqlConnectionManager::removeConnections()
{
     QMutexLocker lock( &mutex_ );
     if( writeConnectionIsFree_ )
     {
          delete writeConnection_;
     }
     else
     {
          qCritical() << "Write connection is not free";
          return false;
     }

     if( !activeReadConnection_.isEmpty() )
     {
          qCritical() << "Some read connection is not returned: " << activeReadConnection_.size();
          return false;
     }
     for( auto connection: notActiveReadConnection_ )
     {
          delete connection;
     }
     return true;
}


SqlWrapper* SqlConnectionManager::getReadOnlyConnection()
{
     QMutexLocker lock( &mutex_ );
     SqlWrapper* result;
     if( !notActiveReadConnection_.isEmpty() )
     {
          result = notActiveReadConnection_.first();
          notActiveReadConnection_.pop_front();
     }
     else
     {
          result = new SqlWrapper( pathToDb_, connectionNumber_++ );
     }
     activeReadConnection_.push_back( result );
     return result;
}


SqlWrapper* SqlConnectionManager::getWriteConnection( bool critical )
{
     if( critical )
     {
          criticalWriteRequestCount_++;
     }

     while( true )
     {
          QMutexLocker lock( &mutex_ );
          if( !writeConnectionIsFree_ )
          {
               condWrite_.wait( &mutex_ );
               continue;
          }
          if( criticalWriteRequestCount_ != 0 && !critical )
          {
               continue;
          }
          // можем захватывать соединение
          if( critical )
          {
               criticalWriteRequestCount_--;
          }
          writeConnectionIsFree_ = false;
          return writeConnection_;
     }
}


void SqlConnectionManager::returnConnection( SqlWrapper* wrapper )
{
     QMutexLocker lock( &mutex_ );
     if( !inited_ )
     {
          return;
     }
     if( wrapper == writeConnection_ )
     {
          writeConnectionIsFree_ = true;
          return;
     }
     activeReadConnection_.removeOne( wrapper );
     notActiveReadConnection_.push_back( wrapper );
}


SqlWrapperProtection::SqlWrapperProtection( SqlWrapper* wrapper )
:wrapper_( wrapper )
{

}


SqlWrapperProtection::~SqlWrapperProtection()
{
     if( wrapper_->isTransactionStarted() )
     {
          wrapper_->rollbackTransaction();
     }
     SqlConnectionManager::returnConnection( wrapper_ );
}


SqlWrapper* SqlWrapperProtection::operator*()
{
     return wrapper_;
}


SqlWrapper* SqlWrapperProtection::operator->()
{
     return wrapper_;
}


SqlWrapper::SqlWrapper( const QString& pathToDb, quint32 connectionNumber_, bool readOnly )
:readOnly_( readOnly )
{
     if( readOnly )
     {
          db_ = QSqlDatabase::addDatabase( "QSQLITE", QString( "SQLiteWrapper" ) + "ReadOnly" + QString::number( connectionNumber_ ) );
     }
     else
     {
          db_ = QSqlDatabase::addDatabase( "QSQLITE", QString( "SQLiteWrapper" ) + "ReadWrite" + QString::number( connectionNumber_ ) );
     }
     db_.setDatabaseName( pathToDb );
     if( !db_.open() )
     {
          qCritical() << "Cannot open db with error" << db_.lastError();
     }
}


SqlWrapper::~SqlWrapper()
{
     QSqlDatabase::removeDatabase( db_.connectionName() );
}


bool SqlWrapper::initDb()
{
     if( readOnly_ )
     {
          qCritical() << "Try use write function on readonly connection" << db_.connectionName();
          return false;
     }
     beginTransaction();
     bool ok = true;
     QSqlQuery query( db_ );
     if( !query.exec( "CREATE TABLE IF NOT EXISTS clients( client_id INTEGER NOT NULL, UUID BLOB NOT NULL );" ) )
     {
          qCritical() << "Cannot create clients table" << query.lastError();
          ok = false;
     }
     if( ok && !query.exec( "CREATE TABLE IF NOT EXISTS types( agent_sensor_id INTEGER NOT NULL, type INTEGER NOT NULL );" ) )
     {
          qCritical() << "Cannot create types table" << query.lastError();
          ok = false;
     }
     if( ok && !query.exec( "CREATE TABLE IF NOT EXISTS data( client_id INTEGER NOT NULL, sensor_id INTEGER NOT NULL, "
                            "time INTEGER NOT NULL, msecs INTEGER, data BLOB NOT NULL );" ) )
     {
          qCritical() << "Cannot create data table" << query.lastError();
          ok = false;
     }
     if( ok )
     {
          commitTransaction();
     }
     else
     {
          rollbackTransaction();
     }
     return ok;
}


bool SqlWrapper::beginTransaction()
{
     if( transactionStarted_ )
     {
          qCritical() << "Try start transaction with already started transaction";
          return false;
     }

     transactionStarted_ = true;
     return db_.transaction();
}


bool SqlWrapper::commitTransaction()
{
     if( !transactionStarted_ )
     {
          qCritical() << "Try commit without begin transaction";
          return false;
     }
     transactionStarted_ = false;
     return db_.commit();
}


bool SqlWrapper::rollbackTransaction()
{
     if( !transactionStarted_ )
     {
          qCritical() << "Try rollback without begin transaction";
          return false;
     }
     transactionStarted_ = false;
     return db_.rollback();
}


bool SqlWrapper::isTransactionStarted() const
{
     return transactionStarted_;
}


bool SqlWrapper::addCreds( quint32 clientId, const QByteArray& uuid )
{
     QSqlQuery query ( db_ );
     query.prepare( "INSERT INTO clients(client_id, UUID) VALUES(:id, :uuid);" );
     query.bindValue( ":id", clientId );
     query.bindValue( ":uuid", uuid );
     return query.exec();
}


QList< QPair< quint32, QByteArray > > SqlWrapper::existClient()
{
     QList< QPair< quint32, QByteArray > > result;
     QSqlQuery query( db_ );
     if( !query.exec( "SELECT client_id, uuid FROM clients;" ) )
     {
          qCritical() << "Cannot get clients with error: " << query.lastError();
          return result;
     }
     while( query.next() )
     {
          result.push_back( qMakePair( query.value( 0 ).toUInt(), query.value( 1 ).toByteArray() ) );
     }
     return result;
}


QList< QPair< quint64, quint8 > > SqlWrapper::registeredDataTypes()
{
     QList< QPair< quint64, quint8 > > result;
     QSqlQuery query( db_ );
     if( !query.exec( "SELECT agent_sensor_id, type FROM types" ) )
     {
          qCritical() << "Error get data types with error:" << query.lastError();
          return result;
     }
     while( query.next() )
     {
          quint64 tmp = ( quint64 )query.value( 0 ).toLongLong();
          result.push_back( qMakePair( tmp, query.value( 1 ).toUInt() ) );
     }
     return result;
}


bool SqlWrapper::registerNewDataType( quint64 id, quint8 dataType )
{
     QSqlQuery query( db_ );
     query.prepare( "INSERT INTO types( agent_sensor_id, type ) VALUES( :id, :data_type );");
     qint64 tmp = ( qint64 )id;
     query.bindValue( ":id", tmp );
     query.bindValue( ":data_type", dataType );
     return query.exec();
}


bool SqlWrapper::writeEvent( quint32 clientId, quint32 sensorId, quint64 time, const QVariant& msec, const QByteArray& data )
{
     QSqlQuery query( db_ );
     query.prepare( "INSERT INTO data(client_id, sensor_id, time, msecs, data) "
                    "VALUES( :client_id, :sensor_id, :time, :msecs, :data );" );
     query.bindValue( ":client_id", clientId );
     query.bindValue( ":sensor_id", sensorId );
     query.bindValue( ":time", time );
     if( msec.isValid() )
     {
          query.bindValue( ":msecs", msec.toUInt() );
     }
     else
     {
          query.bindValue( ":msecs", QVariant() );
     }
     query.bindValue( ":data", data );
     return query.exec();
}