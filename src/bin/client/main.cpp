#include "agent_connection_client.h"
//#include "cmath"

//float f = 50; // частота в Герцах
//float omega = 2 * M_PI * f; // угловая частота
//
//
//float v( quint64 sec, quint8 msec )
//{
//     double t = sec + ( float )msec / 1000;
//     return ( 220 * std::sqrt( 2 ) * sinf( omega * t ) );
//}


int main( int argc, char** argv )
{
     QCoreApplication app( argc, argv );
     AgentConnectionClient cli( "127.0.0.1", 23456 );

     //cli.setAgentId( 2, "ZyyqxlQrR7bxFdQDLECAl1wTZPoaBbN3" );
     cli.onConnect();
     qDebug() << cli.connectedState();
     quint64 curTime = QDateTime::currentSecsSinceEpoch();
     float curVal = 50;
     for( int a = 0; a < 500; a++ )
     {
          cli.onAddEvent( AgentEvent( 0, curTime, 0,  QVariant::fromValue( ( float )70) ) );
          if( curVal < 80 )
          {
               curVal += 0.2;
          }
          cli.onAddEvent( AgentEvent( 1, curTime, 0,  QVariant::fromValue( curVal ) ) );
          curTime++;
     }
     cli.onFlushEvents();
     return app.exec();
}
