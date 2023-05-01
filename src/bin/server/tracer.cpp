#include "tracer.h"


Tracer::Tracer( EventWriter* writer, const QString& path, quint32 sec )
:path_( path ), writer_( writer )
{
     startTime_ = QDateTime::currentSecsSinceEpoch();
     timer_.setInterval( sec * 1000 );
     QFile file( path );
     if( !file.open( QIODevice::WriteOnly ) )
     {
          qDebug() << "Trace error!";
          return;
     }
     file.write( "Время с момента запуска программы(c),Состояние очереди(кол-во)\n" );
     file.waitForBytesWritten( 1000 );
     file.close();
     connect( &timer_, &QTimer::timeout, this, &Tracer::onTimerShot );
     timer_.start();

}

void Tracer::onTimerShot()
{
     quint64 count = writer_->currentQueueCount();
     QFile file( path_ );
     if( !file.open( QIODevice::WriteOnly | QIODevice::Append ) )
     {
          qDebug() << "Trace cannot write data!";
          return;
     }
     QTextStream stream( &file );
     stream << QDateTime::currentSecsSinceEpoch() - startTime_ << "," << count << "\n";
     stream.flush();
}
