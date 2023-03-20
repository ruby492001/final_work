#include "agent_connection_client.h"




int main( int argc, char** argv )
{
     QCoreApplication app( argc, argv );
     AgentConnectionClient cli( "127.0.0.1", 23456 );

     //cli.setAgentId( 2, "ZyyqxlQrR7bxFdQDLECAl1wTZPoaBbN3" );
     cli.onConnect();
     qDebug() << cli.connectedState();
     /// 2'000'000
     for( int a = 0; a < 200'000; a++ )
     {
          quint64 currentTime = QDateTime::currentSecsSinceEpoch();
          cli.onAddEvent( AgentEvent( 0, currentTime,        QTime::currentTime().msec(), QVariant::fromValue( true ) ) );
          cli.onAddEvent( AgentEvent( 1, currentTime + 1 ,   QTime::currentTime().msec(), QVariant::fromValue( false ) ) );
          cli.onAddEvent( AgentEvent( 2, currentTime + 2,    QTime::currentTime().msec(), QVariant::fromValue( quint32( 123456789 ) ) ) );
          cli.onAddEvent( AgentEvent( 3, currentTime + 3,    QTime::currentTime().msec(), QVariant::fromValue( qint32( -123456789 ) ) ) );
          cli.onAddEvent( AgentEvent( 4, currentTime + 4,    QTime::currentTime().msec(), QVariant::fromValue( ( float )1234.15 ) ) );
     }
     cli.onFlushEvents();
     return app.exec();
}
