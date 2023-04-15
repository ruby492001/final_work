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
     if( !query.exec( "SELECT types.agent_sensor_id, aliases.user_name FROM types LEFT JOIN "
                      "aliases ON aliases.agent_sensor_id = types.agent_sensor_id;" ) )
     {
          qCritical() << "Cannot select from aliases:" << query.lastError();
          return {};
     }
     while( query.next() )
     {
          SensorParam tmp{ query.value( 0 ).toLongLong(), query.value( 1 ).toString() };
          res.push_back( std::move( tmp ) );
     }
     return res;
}