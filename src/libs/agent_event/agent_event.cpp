#include "agent_connection_client.h"
#include "agent_event.h"


AgentEvent::AgentEvent( quint32 sensorId, quint64 time, quint16 msecs, const QVariant& value )
:QObject(), sensorId_( sensorId ), time_( time ), msecs_( msecs )
{
     switch( value.typeId() )
     {
          case QMetaType::UInt:
          {
               type_ = AgentEventTypeUInt32;
               quint32 tmp = value.toUInt();
               memcpy( data_, &tmp, sizeof( tmp ) );
               break;
          }
          case QMetaType::Int:
          {
               type_ = AgentEventTypeInt32;
               qint32 tmp = value.toInt();
               memcpy( data_, &tmp, sizeof( tmp ) );
               break;
          }
          case QMetaType::Float:
          {
               type_ = AgentEventTypeFloat;
               float tmp = value.toFloat();
               memcpy( data_, &tmp, sizeof( tmp ) );
               break;
          }
          case QMetaType::Bool:
          {
               type_ = AgentEventTypeBool;
               // чтобы отдельно не занулять
               quint32 tmp = value.toBool();
               memcpy( data_, &tmp, sizeof( tmp ) );
               break;
          }
          default:
               Q_ASSERT_X( false, "", "Invalid data types" );
     }
}


AgentEvent::AgentEvent( quint32 sensorId, quint64 time, quint16 msecs, AgentEventType type, const unsigned char* data )
:QObject(), sensorId_( sensorId ), time_( time ), msecs_( msecs ), type_( type )
{
     memcpy( data_, data, MAX_EVENT_TYPE_SIZE );
}


AgentEvent::AgentEvent( const AgentEvent& rhs )
:QObject( nullptr )
{
     *this = rhs;
}


AgentEvent& AgentEvent::operator=( const AgentEvent& rhs )
{
     if( !rhs.isValid() )
     {
          type_ = AgentEventUndefined;
          return *this;
     }
     else
     {
          type_ = rhs.type_;
     }
     sensorId_ = rhs.sensorId_;
     time_ = rhs.time_;
     msecs_ = rhs.msecs_;
     memcpy( data_, rhs.data_, MAX_EVENT_TYPE_SIZE );
     return *this;
}


bool AgentEvent::isValid() const
{
     if( type_ == AgentEventUndefined )
     {
          return false;
     }
     return true;
}


quint32 AgentEvent::sensorId() const
{
     return sensorId_;
}


quint64 AgentEvent::time() const
{
     return time_;
}


quint16 AgentEvent::msecs() const
{
     return msecs_;
}


AgentEventType AgentEvent::type() const
{
     return type_;
}


float AgentEvent::floatValue() const
{
     Q_ASSERT( type_ == AgentEventTypeFloat );
     return *( ( float* ) data_ );
}


quint32 AgentEvent::uint32Value() const
{
     Q_ASSERT( type_ == AgentEventTypeUInt32 );
     return *( ( quint32* ) data_ );
}


qint32 AgentEvent::int32Value() const
{
     Q_ASSERT( type_ == AgentEventTypeInt32 );
     return *( ( qint32* ) data_ );
}


bool AgentEvent::boolValue() const
{
     Q_ASSERT( type_ == AgentEventTypeBool );
     return *( ( qint32* ) data_ );
}


void AgentEvent::setValue( bool value )
{
     type_ = AgentEventTypeBool;
     quint32 tmp = value;
     memcpy( data_, &tmp, MAX_EVENT_TYPE_SIZE );
}


void AgentEvent::setValue( quint32 value )
{
     type_ = AgentEventTypeUInt32;
     memcpy( data_, &value, MAX_EVENT_TYPE_SIZE );
}


void AgentEvent::setValue( qint32 value )
{
     type_ = AgentEventTypeInt32;
     memcpy( data_, &value, MAX_EVENT_TYPE_SIZE );
}


void AgentEvent::setValue( float value )
{
     type_ = AgentEventTypeFloat;
     memcpy( data_, &value, MAX_EVENT_TYPE_SIZE );
}

AgentEvent::AgentEvent( quint32 sensorId, quint64 time, quint16 msecs, bool value )
:sensorId_( sensorId ), time_( time ), msecs_( msecs ), type_( AgentEventTypeBool )
{
     quint32 tmp = value;
     memcpy( data_, &tmp, MAX_EVENT_TYPE_SIZE );
}


bool AgentEvent::msecsExist() const
{
     return msecs_ != UNDEFINED_MSECS;
}
