#include "agent_connection_client.h"
#include "thread_wrapper.h"


int main( int argc, char** argv )
{
     QCoreApplication app( argc, argv );
     QStringList args = app.arguments();
     args.pop_front();
     quint64 countPerSecond = 100'000;
     quint64 threadCount = 3;
     if( args.count() >= 2 )
     {
          threadCount = args.at( 0 ).toULongLong();
          countPerSecond = args.at( 1 ).toULongLong();
     }
     LoadThreadWrapper wrapper( threadCount, countPerSecond );

     return app.exec();
}
