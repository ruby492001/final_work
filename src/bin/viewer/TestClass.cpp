#include "TestClass.h"

TestClass::TestClass()
{
     {
          client = new ViewConnectionClient( "127.0.0.1", 54321 );
          connect( client, &ViewConnectionClient::sConnected, this, &TestClass::onConnected );
          connect( client, &ViewConnectionClient::sConnectionError, this, &TestClass::onConnectionError );
          connect( client, &ViewConnectionClient::sResponseReceived, this, &TestClass::onEchoGet );
          client->onConnect();
     }
}


void TestClass::onConnected()
{
     QByteArray arr = QByteArray::fromHex( "0123456789ABCDEF" );
     ViewArgs arg;

     QList<ViewArgs> args;
     args.push_back( arg );

     arg.addVariable( QVariant::fromValue( 1234 ) );
     args.push_back( arg );

     arg.addVariable( QVariant::fromValue( -4321 ) );
     args.push_back( arg );

     arg.addVariable( QString( "Hello world" ) );
     args.push_back( arg );

     arg.addVariable( QVariant::fromValue( true ) );
     args.push_back( arg );

     arg.addVariable( arr );
     args.push_back( arg );


     ViewCommand cmd( 1234, 4321, args );


     client->onSendCommand( cmd );
     qDebug() << "TestClass: connected";

}


void TestClass::onConnectionError()
{
     qDebug() << "TestClass: connection error";
}


void TestClass::onConnectionLost()
{
     qDebug() << "TestClass: connection lost";
}


void TestClass::onEchoGet( const ViewCommand& cmd )
{
     qDebug() << cmd.isValid();
     qDebug() << cmd.plgId() << cmd.cmd() << cmd.count() << "\n\n";
     for( quint32 currentIdx = 0; currentIdx < cmd.count(); currentIdx++ )
     {
          qDebug() << "Arg number: " << currentIdx << "Arg count:" << cmd.at( currentIdx )->count();
          for( quint32 idx = 0; idx < cmd.at( currentIdx )->count(); idx++ )
          {
               if( idx == 4 )
               {
                    qDebug() << cmd.at( currentIdx )->at( idx )->toByteArray().toHex();
                    continue;
               }
               qDebug() << *cmd.at( currentIdx )->at( idx );

          }
          qDebug() << "\n\n";

     }
}
