#include "view_data.h"
#include "db_wrapper_view_data.h"
#include "db_wrapper.h"


quint32 EditSensorSrv::pluginId()
{
     return VIEW_PLG_ID;
}


bool EditSensorSrv::init()
{
     return true;
}


bool EditSensorSrv::initDbTables()
{
     return true;
}


void EditSensorSrv::onNetRequest( const ViewCommand& cmd )
{
     if( cmd.plgId() != pluginId() )
     {
          return;
     }
     switch( cmd.cmd() )
     {
          case VD_GET_SENSOR_REQ:
          {
               onGetValues();
               break;
          }
          case VD_GET_DATA_REQ:
          {
               onGetData( cmd );
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
     ViewDataWrapper wrapper( sqlWrapper );

     auto res = wrapper.getParams();
     qDebug() << "EditSensorSrv::onGetValues count:" << res.count();
     ViewCommand cmd( pluginId(), VD_GET_SENSOR_RES );
     foreach( const auto cur, res )
     {
          ViewArgs arg;
          arg.addVariable( cur.clientSensorId );
          arg.addVariable( cur.userName );
          cmd.addArg( arg );
     }
     emit sSendResponse( cmd );
}


void EditSensorSrv::onGetData( const ViewCommand& cmd )
{
     SqlWrapperProtection wrapper( SqlConnectionManager::getReadOnlyConnection() );
     QList< QList< QPair< quint64, double > > > res;
     if( cmd.count() < 3 )
     {
          return;
     }
     quint64 startTime = cmd.at( 0 )->at( 0 )->toULongLong();
     quint64 endTime = cmd.at( 1 )->at( 0 )->toULongLong();
     ViewCommand toClient( pluginId(), VD_GET_DATA_RES );
     for( quint32 idx = 2; idx < cmd.count(); idx++ )
     {
          QList< QPair< quint64, double > > tmp = wrapper->getData( cmd.at( idx )->at( 0 )->toULongLong(), startTime, endTime );
          ViewArgs args;
          args.addVariable( cmd.at( idx )->at( 0 )->toULongLong() );
          foreach( const auto& curElement, tmp )
          {
               args.addVariable( curElement.first );
               args.addVariable( curElement.second );
          }
          toClient.addArg( args );
     }
     emit sSendResponse( toClient );
}
