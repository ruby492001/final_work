#pragma once
#include "db_wrapper.h"
#include "edit_sensor_defs.h"


class EditSensorWrapper: public QObject
{
     Q_OBJECT
public:
     EditSensorWrapper( SqlWrapper* writeWrapper );
     bool initEditSensorTableSchema();
     bool addOrUpdateUserName( const SensorParam& param );

     /// @brief получить псевдонимы для всех датчиков всех клиентов, даже если для них нет существующих всевдонимов
     QList< SensorParam > getParams();

private:
     QSqlDatabase* db_;
};