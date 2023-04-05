#include "edit_sensor.h"
#include "db_wrapper_edit_sensor.h"


quint32 EditSensorSrv::pluginId()
{
     return EDIT_SENSOR_PARAMETER;
}


bool EditSensorSrv::init()
{
     return true;
}


bool EditSensorSrv::initDbTables()
{
     SqlWrapper* sqlWrapper = SqlConnectionManager::getWriteConnection();
     SqlWrapperProtection protection( sqlWrapper );
     EditSensorWrapper wrapper( sqlWrapper );

     protection->beginTransaction();
     inited_ = wrapper.initEditSensorTableSchema();
     if( inited_ )
     {
          protection->commitTransaction();
          return true;
     }
     else
     {
          protection->rollbackTransaction();
          return false;
     }
}


void EditSensorSrv::onNetRequest( const ViewCommand& cmd )
{
     if( cmd.plgId() != pluginId() )
     {
          return;
     }
     switch( cmd.cmd() )
     {
          case ES_GET_VALUES_REQ:
          {
               onGetValues();
               break;
          }
          case ES_SET_VALUES_REQ:
          {
               auto params = parseCmdToSensorParam( cmd );
               onSetValues( params );
               break;
          }
          default:
          {
               qCritical() << "Incorrect command:" << cmd.cmd();
          }
     }
}


void EditSensorSrv::onGetValues()
{
     SqlWrapper* sqlWrapper = SqlConnectionManager::getReadOnlyConnection();
     SqlWrapperProtection protection( sqlWrapper );
     EditSensorWrapper wrapper( sqlWrapper );

     auto res = wrapper.getParams();
     qDebug() << "EditSensorSrv::onGetValues count:" << res.count();
     ViewCommand cmd( pluginId(), ES_GET_VALUES_RES );
     foreach( const auto cur, res )
     {
          ViewArgs arg;
          arg.addVariable( cur.clientSensorId );
          arg.addVariable( ( quint32 )cur.eventType );
          arg.addVariable( cur.userName );
          cmd.addArg( arg );
     }
     emit sSendResponse( cmd );
}


QList< SensorParam > EditSensorSrv::parseCmdToSensorParam( const ViewCommand& cmd )
{
     QList< SensorParam > res;
     for( quint32 idx = 0; idx < cmd.count(); idx++ )
     {
          const ViewArgs* arg = cmd.at( idx );
          SensorParam tmp{ arg->at( 0 )->toLongLong(), AgentEventUndefined, arg->at( 1 )->toString() };
          res.push_back( tmp );
     }
     return res;
}


void EditSensorSrv::onSetValues( const QList<SensorParam>& params )
{
     SqlWrapper* sqlWrapper = SqlConnectionManager::getWriteConnection();
     SqlWrapperProtection protection( sqlWrapper );
     protection->beginTransaction();
     EditSensorWrapper wrapper( sqlWrapper );
     foreach( const auto& cur, params )
     {
          wrapper.addOrUpdateUserName( cur );
     }
     protection->commitTransaction();
}

