#pragma once
#include "db_wrapper.h"
#include "view_data_defs.h"


class ViewDataWrapper: public QObject
{
     Q_OBJECT
public:
     ViewDataWrapper( SqlWrapper* writeWrapper );
     QList< SensorParam > getParams();

private:
     QSqlDatabase* db_;
};