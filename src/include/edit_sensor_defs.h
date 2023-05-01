#pragma once


#include <QtCore>

#include "agent_event.h"

struct SensorParam
{
     qint64 clientSensorId;
     AgentEventType eventType;
     QString userName;
     QString place;
     QString type;
     QString measure;
};

#define ES_GET_VALUES_REQ 1
#define ES_GET_VALUES_RES ( ES_GET_VALUES_REQ << 16 )

#define ES_SET_VALUES_REQ 2