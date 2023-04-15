#include "view_data.h"
#include "plg_ids.h"
QString ViewDataView::name() const
{
     return tr( "Просмотр данных" );
}


quint32 ViewDataView::id() const
{
     return VIEW_PLG_ID;
}


quint32 ViewDataView::priority() const
{
     return 1;
}


void ViewDataView::init( QSqlDatabase* )
{
}


QWidget* ViewDataView::widget( QWidget* parent )
{
     wgt_ = new ViewDataWidget( parent );
     connect( wgt_, &ViewDataWidget::sGetSensorRequest, this, &ViewDataView::onWidgetRequestSensor );
     connect( wgt_, &ViewDataWidget::sGetData, this, &ViewDataView::onWidgetRequestData );
     connect( this, &ViewDataView::sWidgetResponseSensor, wgt_, &ViewDataWidget::onGetSensorResponse );
     connect( this, &ViewDataView::sWidgetResponseData, wgt_, &ViewDataWidget::onGetDataResponse );
     return wgt_;
}


void ViewDataView::onNetResponse( const ViewCommand& cmd )
{
     if( cmd.plgId() != id() )
     {
          return;
     }
     switch( cmd.cmd() )
     {
          case VD_GET_SENSOR_RES:
          {
               parseResponseSensor( cmd );
               break;
          }
          case VD_GET_DATA_RES:
          {
               parseResponseData( cmd );
               break;
          }
          default:
          {
               qCritical() << "ViewDataView: Incorrect response command id:" << cmd.cmd();
          }
     }
}


QIcon ViewDataView::icon() const
{
     return QIcon( ":/images/graph.png" );
}


void ViewDataView::onWidgetRequestSensor()
{
     ViewCommand cmd( id(), VD_GET_SENSOR_REQ );
     emit sSendNetRequest( cmd );
}


void ViewDataView::parseResponseSensor( const ViewCommand& cmd )
{
     QList<SensorParam> sensors;
     for( quint32 curIdx = 0; curIdx < cmd.count(); curIdx++ )
     {
          sensors.push_back( { cmd.at( curIdx )->at( 0 )->toLongLong(), cmd.at( curIdx )->at( 1 )->toString() } );
     }
     emit sWidgetResponseSensor( sensors );
}


void ViewDataView::onWidgetRequestData( const QList<qint64>& data )
{
     ViewCommand cmd( id(), VD_GET_DATA_REQ );
     foreach( quint64 cur, data )
     {
          ViewArgs args;
          args.addVariable( cur );
          cmd.addArg( args );
     }
     emit sSendNetRequest( cmd );
}


void ViewDataView::parseResponseData( const ViewCommand& cmd )
{
     QList< DataFormat > data;
     for( quint32 idx = 0; idx < cmd.count(); idx++ )
     {
          DataFormat tmp;
          const ViewArgs* curArg = cmd.at( idx );
          tmp.id = curArg->at( 0 )->toULongLong();
          for( quint32 dataIdx = 1; dataIdx < curArg ->count() - 1; dataIdx += 2 )
          {
             tmp.timeWithMsec.push_back( curArg->at( dataIdx )->toULongLong() );
             tmp.values.push_back( *curArg->at( dataIdx + 1 ) );
          }
          data.push_back( tmp );
     }
     emit sWidgetResponseData( data );
}
