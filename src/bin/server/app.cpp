#include "app.h"

ServerApp::ServerApp( int argc, char** argv )
:QCoreApplication( argc, argv )
{
     server_ = new AgentConnectionServer( 23456 );
}


int ServerApp::start()
{
     if( server_->startListening() )
     {
          return QCoreApplication::exec();
     }
     return -1;
}


