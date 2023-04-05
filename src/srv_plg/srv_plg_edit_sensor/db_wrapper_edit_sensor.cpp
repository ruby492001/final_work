#include "db_wrapper_edit_sensor.h"
#include "QSqlError"


EditSensorWrapper::EditSensorWrapper( SqlWrapper* writeWrapper )
{
     qDebug() << "EditSensorWrapper inited";
     db_ = writeWrapper->db();
}


bool EditSensorWrapper::initEditSensorTableSchema()
{
     QSqlQuery query( *db_ );
     bool result = query.exec( "CREATE TABLE IF NOT EXISTS aliases( agent_sensor_id INTEGER NOT NULL UNIQUE, user_name STRING NOT NULL );" );
     if( !result )
     {
          qCritical() << "Cannot init edit sensor table:" << query.lastError();
     }
     return result;
}


bool EditSensorWrapper::addOrUpdateUserName( const SensorParam& param )
{
     QSqlQuery query( *db_ );
     QSqlQuery writeQuery( *db_ );
     query.prepare( "SELECT * FROM aliases WHERE agent_sensor_id=:id;" );
     query.bindValue( ":id", param.clientSensorId );
     if( !query.exec() )
     {
          qCritical() << "Cannot select value in aliases table:" << query.lastError();
          return false;
     }
     if( !query.next() )
     {
          // псевдонима для этого id не существует
          writeQuery.prepare( "INSERT INTO aliases( agent_sensor_id, user_name ) VALUES( :id, :user_name );" );
          writeQuery.bindValue( ":id", param.clientSensorId );
          writeQuery.bindValue( ":user_name", param.userName );
          if( !writeQuery.exec() )
          {
               qCritical() << "Cannot insert value in aliases table:" << query.lastError();
               return false;
          }
     }
     else
     {
          // псевдоним уже существует
          writeQuery.prepare( "UPDATE aliases SET user_name = :user_name WHERE agent_sensor_id=:id;" );
          writeQuery.bindValue( ":id", param.clientSensorId );
          writeQuery.bindValue( ":user_name", param.userName );
          if( !writeQuery.exec() )
          {
               qCritical() << "Cannot update value in aliases table:" << query.lastError();
          }
     }
     return true;
}


QList< SensorParam > EditSensorWrapper::getParams()
{
     QList< SensorParam > res;
     //QList< qint64 > existsInAliases;
     QSqlQuery query( *db_ );
     if( !query.exec( "SELECT types.agent_sensor_id, types.type, aliases.user_name FROM types LEFT JOIN "
                      "aliases ON aliases.agent_sensor_id = types.agent_sensor_id;" ) )
     {
          qCritical() << "Cannot select from aliases:" << query.lastError();
          return {};
     }
     while( query.next() )
     {
          SensorParam tmp{ query.value( 0 ).toLongLong(), ( AgentEventType )query.value( 1 ).toInt(), query.value( 2 ).toString() };
          //existsInAliases.push_back( tmp.clientSensorId );
          res.push_back( std::move( tmp ) );
     }
     return res;
}