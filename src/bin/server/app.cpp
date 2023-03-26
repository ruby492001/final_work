#include "app.h"
#include "cred_validator.h"

ServerApp::ServerApp( int argc, char** argv )
:QCoreApplication( argc, argv )
{
     manager_ = new SqlConnectionManager( "test_db.db" );
     CredValidator::init();
     eventWriter = new EventWriter( 100'000'000, 500'000'000 );
     server_ = new AgentConnectionServer( 23456 );
     viewServer_ = new ViewConnectionServer( 54321 );
}


int ServerApp::start()
{
     if( server_->startListening() && viewServer_->startListening() )
     {
          return QCoreApplication::exec();
     }
     return -1;
}


