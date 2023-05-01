#include "thread_wrapper.h"


LoadThread::LoadThread( quint64 eventCount, quint32 num )
:num_( num )
{
     quint64 currentTime = QDateTime::currentSecsSinceEpoch();
     quint32 idx;
     for( idx = 0; idx < eventCount / 5; idx++ )
     {
          events_.push_back( AgentEvent( idx + 0, currentTime,        QTime::currentTime().msec(), QVariant::fromValue( true ) ) );
          events_.push_back( AgentEvent( idx + 1, currentTime + 1 ,   QTime::currentTime().msec(), QVariant::fromValue( false ) ) );
          events_.push_back( AgentEvent( idx + 2, currentTime + 2,    QTime::currentTime().msec(), QVariant::fromValue( quint32( 123456789 ) ) ) );
          events_.push_back( AgentEvent( idx + 3, currentTime + 3,    QTime::currentTime().msec(), QVariant::fromValue( qint32( -123456789 ) ) ) );
          events_.push_back( AgentEvent( idx + 4, currentTime + 4,    QTime::currentTime().msec(), QVariant::fromValue( ( float )1234.15 ) ) );
     }
     while( events_.count() < ( long long )eventCount )
     {
          events_.push_back( AgentEvent( idx, currentTime + 4,    QTime::currentTime().msec(), QVariant::fromValue( ( float )1234.15 ) ) );
          idx++;
     }

     qDebug() << num << "inited!";
}


void LoadThread::onSend()
{
     if( !client_ )
     {
          client_ = new AgentConnectionClient( "127.0.0.1", 23456 );
          client_->onConnect();
          if( client_->connectedState() != AgentConnectionClientActive )
          {
               qDebug() << "Test thread" << num_ << client_->connectedState() << "state!";
               return;
          }
          qDebug() << "Test thread" << num_ << "state OK";
          foreach( const auto& event, events_ )
          {
               client_->onAddEvent( event );
          }
     }
     client_->onFlushEvents();
     quint64 currentTime = QDateTime::currentSecsSinceEpoch();
     for( auto a = events_.begin(); a != events_.end(); a++ )
     {
          a->setTime( currentTime );
          client_->onAddEvent( *a );
     }
     qDebug() << num_ << "send" << events_.count() << ". Connection state" << client_->connectedState();
}


LoadThreadWrapper::LoadThreadWrapper( quint64 threadCount, quint64 eventCount )
{
     for( quint64 idx = 0; idx < threadCount; idx++ )
     {
          QThread* trd = new QThread();
          LoadThread* load = new LoadThread( eventCount, idx + 1 );
          load->moveToThread( trd );
          connect( &timer, &QTimer::timeout, load, &LoadThread::onSend );
          threads_.push_back( trd );
          workers_.push_back( load );
          trd->start();
     }
     timer.setInterval( 1000 );
     timer.start();
}
