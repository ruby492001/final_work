#include "app.h"
#include "cred_validator.h"

ServerApp::ServerApp( int argc, char** argv )
:QCoreApplication( argc, argv )
{
     manager = new SqlConnectionManager( "test_db.db" );
     CredValidator::init();
     eventWriter = new EventWriter( 100'000'000, 500'000'000 );
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


