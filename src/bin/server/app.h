#pragma once

#include <QCoreApplication>

#include "agent_connection_server.h"
#include "db_wrapper.h"
#include "event_writer.h"
#include "view_connection_server.h"
#include "tracer.h"


class ServerApp: public QCoreApplication
{
     Q_OBJECT
public:
     ServerApp( int argc, char** argv );

     int start();

private:
     AgentConnectionServer* server_;
     SqlConnectionManager* manager_;
     ViewConnectionServer* viewServer_;
     bool help = false;
     Tracer* tracer_;
};
