#include "db_wrapper_view_data.h"
#include "QSqlError"


ViewDataWrapper::ViewDataWrapper( SqlWrapper* writeWrapper )
{
     db_ = writeWrapper->db();
}


QList< SensorParam > ViewDataWrapper::getParams()
{
     QList< SensorParam > res;
     QSqlQuery query( *db_ );
     if( !query.exec( "SELECT types.agent_sensor_id, sensor_info.user_name, sensor_info.place,"
                      "sensor_info.type, sensor_info.measure FROM types LEFT JOIN "
                      "sensor_info ON sensor_info.agent_sensor_id = types.agent_sensor_id;" ) )
     {
          qCritical() << "Cannot select from aliases:" << query.lastError();
          return {};
     }
     while( query.next() )
     {
          SensorParam tmp{ query.value( 0 ).toLongLong(), query.value( 1 ).toString(),
                           query.value( 2 ).toString(), query.value( 3 ).toString(), query.value( 4 ).toString()  };
          res.push_back( std::move( tmp ) );
     }
     return res;
}