#pragma once


#include <QtCore>

/// @brief максимальный размер данных
#define MAX_EVENT_TYPE_SIZE 4

/// @brief милисекунд не существуют для события
#define UNDEFINED_MSECS 1000


enum AgentEventType
{
     Undefined,
     AgentEventTypeBool,
     AgentEventTypeFloat,
     AgentEventTypeUInt32
};



class AgentEvent: public QObject
{
     Q_OBJECT
public:
     AgentEvent( quint32 sensorId = 0, quint64 time = 0, quint16 msecs = UNDEFINED_MSECS, const QVariant& value = QVariant() );
     AgentEvent( quint32 sensorId = 0, quint64 time = 0, quint16 msecs = UNDEFINED_MSECS, AgentEventType type = Undefined, unsigned char* data = nullptr, quint8 dataSize = 0 );
     AgentEvent( const AgentEvent& rhs );
     AgentEvent& operator=( const AgentEvent& rhs );

     bool isValid();
     quint32 sensorId();
     quint64 time();
     quint16 msecs();
     AgentEventType type();
     float floatValue();
     quint32 uint32Value();
     bool boolValue();


     void setValue( bool value );
     void setValue( quint32 value );
     void setValue( float value );


private:
     quint32 sensorId_;
     quint64 time_;
     quint16 msecs_;
     unsigned char data_[4];
     quint8 dataSize_;
     AgentEventType type_ = Undefined;
};