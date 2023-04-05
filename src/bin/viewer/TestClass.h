#pragma once

#include <QtCore>
#include "view_connection_client.h"

class TestClass: public QObject
{
     Q_OBJECT
public:
     TestClass();

private slots:
     void onConnected();
     void onConnectionError();
     void onConnectionLost();
     void onEchoGet( const ViewCommand& cmd );
private:
     ViewConnectionClient* client;
};