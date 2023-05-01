#pragma once

#include <QtCore>

struct SensorParam
{
     qint64 clientSensorId;
     QString userName;
     QString place;
     QString type;
     QString measure;
};


#define VD_GET_SENSOR_REQ 1
#define VD_GET_SENSOR_RES ( VD_GET_SENSOR_REQ << 16 )

#define VD_GET_DATA_REQ 2
#define VD_GET_DATA_RES ( VD_GET_DATA_REQ << 16 )
