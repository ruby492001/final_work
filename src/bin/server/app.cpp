#include "app.h"
#include "cred_validator.h"

ServerApp::ServerApp( int argc, char** argv )
:QCoreApplication( argc, argv )
{
     QStringList args = arguments();
     args.pop_front();   // удаляем имя приложения
     if( args.indexOf( "--help" ) != -1 )
     {
          qDebug() << "Справочная информация. \n Программа сбора и хранения данных электротехнического объекта.\n"
                      "Аргументы:\n"
                      "--client_port=порт     - порт для подключения программ отправки данных(по умолчанию - 23456)\n"
                      "--view_port=порт      - порт для подключения программ просмотра(по умолчанию - 54321)\n"
                      "--trace=on/off        - отслеживание размера очереди событий для записи(по умолчанию не используется)\n"
                      "--trace_path='путь'   - файл для записи размера очереди событий(по умолчанию, отсутствует)\n"
                      "--trace_time=секунды  - период записи размера очереди событий(по умолчанию, не используется)\n";
          help = true;
          return;
     }
     //парсим аргументы, если есть
     quint16 agentPort = 23456;
     quint16 viewPort = 54321;
     bool trace = false;
     QString tracePath;
     quint64 traceTime = 0;
     while( !args.isEmpty() )
     {
          QStringList curArg = args.first().split( '=' );
          args.pop_front();
          if( curArg.first() == "--client_port" )
          {
               agentPort = curArg.at( 1 ).toULongLong();
               continue;
          }
          if( curArg.first() == "--view_port" )
          {
               viewPort = curArg.at( 1 ).toULongLong();
               continue;
          }
          if( curArg.first() == "--trace" )
          {
               trace = true;
               continue;
          }
          if( curArg.first() == "--trace_path" )
          {
               tracePath = curArg.at( 1 );
               continue;
          }
          if( curArg.first() == "--trace_time" )
          {
               traceTime = curArg.at( 1 ).toULongLong();
               continue;
          }

          qDebug() << "Неизвестный аргумент:" << curArg.first();
          help = true;
          return;
     }
     if( trace && ( tracePath.isEmpty() || traceTime == 0  ) )
     {
          help = true;
          qDebug() << "Некорректные аргументы для трассировки";
          return;
     }
     manager_ = new SqlConnectionManager( "../etc/event.db" );
     CredValidator::init();
     eventWriter = new EventWriter( 20'000'000, 100'000'000 );
     server_ = new AgentConnectionServer( agentPort );
     viewServer_ = new ViewConnectionServer( viewPort );
     if( trace )
     {
          tracer_ = new Tracer( eventWriter, tracePath, traceTime );
     }
}


int ServerApp::start()
{
     if( help )
     {
          return 0;
     }
     if( server_->startListening() && viewServer_->startListening() )
     {
          return QCoreApplication::exec();
     }
     return -1;
}


