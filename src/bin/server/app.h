#pragma once

#include <QCoreApplication>

#include "agent_connection_server.h"



class ServerApp: public QCoreApplication
{
     Q_OBJECT
public:
     ServerApp( int argc, char** argv );

     int start();

private:
     AgentConnectionServer* server_;
};
