#include "edit_sensor.h"
#include "plg_ids.h"



QString EditSensorView::name() const
{
     return tr( "Редактирование параметров\nдатчиков" );
}


quint32 EditSensorView::id() const
{
     return EDIT_SENSOR_PARAMETER;
}


QIcon EditSensorView::icon() const
{
     return QIcon(":/images/edit_settings.png");
}


void EditSensorView::init()
{
}


QWidget* EditSensorView::widget( QWidget* parent )
{
     wgt_ = new EditSensorConfigWidget( parent );
     connect( wgt_, &EditSensorConfigWidget::sSendChanges, this, &EditSensorView::onSendChanges );
     connect( wgt_, &EditSensorConfigWidget::sUpdateTableRequest, this, &EditSensorView::onUpdateTableRequest );
     connect( this, &EditSensorView::sUpdateTable, wgt_, &EditSensorConfigWidget::onUpdateTable );
     onUpdateTableRequest();
     return wgt_;
}


quint32 EditSensorView::priority() const
{
     return 0;
}


void EditSensorView::onSendChanges( const QList<SensorParam>& cmd )
{
     ViewCommand command( id(), ES_SET_VALUES_REQ );
     for( const auto& cur: cmd )
     {
          ViewArgs tmpArg;
          tmpArg.addVariable( cur.clientSensorId );
          tmpArg.addVariable( cur.userName );
          tmpArg.addVariable( cur.place );
          tmpArg.addVariable( cur.type );
          tmpArg.addVariable( cur.measure );
          command.addArg( tmpArg );
     }
     emit sSendNetRequest( command );
}


void EditSensorView::onUpdateTableRequest()
{
     ViewCommand command( id(), ES_GET_VALUES_REQ );
     emit sSendNetRequest( command );
}


void EditSensorView::onNetResponse( const ViewCommand& cmd )
{
     if( cmd.plgId() != id() )
     {
          return;
     }
     switch( cmd.cmd() )
     {
          case ES_GET_VALUES_RES:
          {
               onUpdateTableRes( cmd );
               break;
          }
          default:
          {
               qWarning() << "Incorrect response" << cmd.cmd();
          }
     }
}


void EditSensorView::onUpdateTableRes( const ViewCommand& cmd )
{
     QList< SensorParam > data;
     for( quint32 idx = 0; idx < cmd.count(); idx++ )
     {
          SensorParam tmp{ cmd.at( idx )->at( 0 )->toLongLong(),
                           ( AgentEventType )cmd.at( idx )->at( 1 )->toInt(),
                           cmd.at( idx )->at( 2 )->toString(),
                           cmd.at( idx )->at( 3 )->toString(),
                           cmd.at( idx )->at( 4 )->toString(),
                           cmd.at( idx )->at( 5 )->toString()};
          data.push_back( tmp );
     }
     emit sUpdateTable( data );
}
