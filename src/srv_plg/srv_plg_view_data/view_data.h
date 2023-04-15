#pragma once


#include "srvplg_itf.h"
#include "plg_ids.h"
#include "db_wrapper_view_data.h"
#include "view_data_defs.h"


class EditSensorSrv: public ServicePluginItf
{
     Q_OBJECT
     Q_INTERFACES( ServicePluginItf )
     Q_PLUGIN_METADATA( IID "FINAL_WORK.ServicePluginItf/1.0" )
public:
     quint32 pluginId() override;
     bool init() override;
     bool initDbTables() override;

public slots:
     void onNetRequest( const ViewCommand& cmd ) override;

signals:
     void sSendResponse( const ViewCommand& cmd );

private:
     void onGetValues();
     void onGetData( const ViewCommand& cmd );
     QList< SensorParam > parseCmdToSensorParam( const ViewCommand& cmd );
     void onSetValues( const QList< SensorParam >& params );

private:
     bool inited_ = false;
};