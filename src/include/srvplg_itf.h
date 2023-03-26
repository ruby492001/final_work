#include <QtCore>
#include "view_cmd.h"


class ServicePluginItf: public QObject
{
public:
     virtual quint32 pluginId() = 0;
     virtual void init() = 0;
     virtual void initDbTables() = 0;

public slots:
     virtual void onNewRequest( const ViewCommand& cmd ) = 0;

signals:
     void sSendResponse( const ViewCommand& cmd );
};

Q_DECLARE_INTERFACE( ServicePluginItf, "FINAL_WORK.ServicePluginItf/1.0" )