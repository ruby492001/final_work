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
     AgentEventTypeUInt32,
     AgentEventTypeInt32
};



class AgentEvent: public QObject
{
     Q_OBJECT
public:
     AgentEvent( quint32 sensorId = 0, quint64 time = 0, quint16 msecs = UNDEFINED_MSECS, const QVariant& value = QVariant() );
     AgentEvent( quint32 sensorId = 0, quint64 time = 0, quint16 msecs = UNDEFINED_MSECS, AgentEventType type = Undefined, const unsigned char* data = nullptr );
     AgentEvent( quint32 sensorId = 0, quint64 time = 0, quint16 msecs = UNDEFINED_MSECS, bool value = false );
     AgentEvent( const AgentEvent& rhs );
     AgentEvent& operator=( const AgentEvent& rhs );

     bool isValid() const;
     quint32 sensorId() const;
     quint64 time() const;
     quint16 msecs() const;
     bool msecsExist() const;
     AgentEventType type() const;
     float floatValue() const;
     quint32 uint32Value() const;
     qint32 int32Value() const;
     bool boolValue() const;


     void setValue( bool value );
     void setValue( quint32 value );
     void setValue( qint32 value );
     void setValue( float value );


private:
     quint32 sensorId_;
     quint64 time_;
     quint16 msecs_ = UNDEFINED_MSECS;
     unsigned char data_[MAX_EVENT_TYPE_SIZE];
     AgentEventType type_ = Undefined;
};