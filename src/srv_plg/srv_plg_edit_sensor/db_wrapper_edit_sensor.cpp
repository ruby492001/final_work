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
     bool result = query.exec( "CREATE TABLE IF NOT EXISTS sensor_info( agent_sensor_id INTEGER NOT NULL UNIQUE, "
                               "user_name TEXT, place TEXT, type TEXT, measure TEXT );" );
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
     query.prepare( "SELECT * FROM sensor_info WHERE agent_sensor_id=:id;" );
     query.bindValue( ":id", param.clientSensorId );
     if( !query.exec() )
     {
          qCritical() << "Cannot select value in sensor_info table:" << query.lastError();
          return false;
     }
     if( !query.next() )
     {
          // псевдонима для этого id не существует
          writeQuery.prepare( "INSERT INTO sensor_info( agent_sensor_id, user_name, place, type, measure )"
                              " VALUES( :id, :user_name, :place, :type, :measure );" );
          writeQuery.bindValue( ":id", param.clientSensorId );
          writeQuery.bindValue( ":user_name", param.userName.isEmpty() ? QString( "NULL" ) :  param.userName );
          writeQuery.bindValue( ":place", param.place.isEmpty() ? QString( "NULL" ) :  param.place );
          writeQuery.bindValue( ":type", param.type.isEmpty() ? QString( "NULL" ) :  param.type );
          writeQuery.bindValue( ":measure", param.measure.isEmpty() ? QString( "NULL" ) :  param.measure );

          if( !writeQuery.exec() )
          {
               qCritical() << "Cannot insert value in sensor_info table:" << query.lastError();
               return false;
          }
     }
     else
     {
          // псевдоним уже существует
          writeQuery.prepare( "UPDATE sensor_info SET user_name = :user_name, "
                              "place = :place, type = :type, measure = :measure"
                              " WHERE agent_sensor_id=:id;" );
          writeQuery.bindValue( ":id", param.clientSensorId );
          writeQuery.bindValue( ":user_name", param.userName.isEmpty() ? QString( "NULL" ) :  param.userName );
          writeQuery.bindValue( ":place", param.place.isEmpty() ? QString( "NULL" ) :  param.place );
          writeQuery.bindValue( ":type", param.type.isEmpty() ? QString( "NULL" ) :  param.type );
          writeQuery.bindValue( ":measure", param.measure.isEmpty() ? QString( "NULL" ) :  param.measure );
          if( !writeQuery.exec() )
          {
               qCritical() << "Cannot update value in sensor_info table:" << query.lastError();
          }
     }
     return true;
}


QList< SensorParam > EditSensorWrapper::getParams()
{
     QList< SensorParam > res;
     //QList< qint64 > existsInAliases;
     QSqlQuery query( *db_ );
     if( !query.exec( "SELECT types.agent_sensor_id, types.type, sensor_info.user_name, "
                      "sensor_info.place, sensor_info.type, sensor_info.measure FROM types LEFT JOIN "
                      "sensor_info ON sensor_info.agent_sensor_id = types.agent_sensor_id;" ) )
     {
          qCritical() << "Cannot select from sensor_info:" << query.lastError();
          return {};
     }
     while( query.next() )
     {
          SensorParam tmp{ query.value( 0 ).toLongLong(), ( AgentEventType )query.value( 1 ).toInt(), query.value( 2 ).toString(),
                           query.value( 3 ).toString(), query.value( 4 ).toString(), query.value( 5 ).toString() };
          //existsInAliases.push_back( tmp.clientSensorId );
          res.push_back( std::move( tmp ) );
     }
     return res;
}