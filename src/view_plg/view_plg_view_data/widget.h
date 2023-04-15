#pragma once

#include <QtGui>
#include <QtWidgets>

#include "qcustomplot.h"
#include "view_data_defs.h"
#include "add_sensor_dialog.h"
#include "view_cmd.h"


struct DataFormat
{
     quint64 id;
     QList< quint64 > timeWithMsec;
     QList< QVariant > values;
};


class ViewDataWidget: public QWidget
{
     Q_OBJECT

public:
     ViewDataWidget( QWidget* parent );

public slots:
     void onGetSensorResponse( const QList<SensorParam>& sensors );
     void onGetDataResponse( const QList< DataFormat >& dataRes );
signals:
     void sGetSensorRequest();
     void sGetData( const QList< qint64 >& ids );

private slots:
     void onAddElementBtn();
     void onContextMenuRequested( const QPoint& pos );
     void onDeleteItems();
     void onCommitSettings();

private:
     QColor getColor( quint64 id );

private:
     QCustomPlot* plot_;
     QDateTimeEdit* startTimeEdit_;
     QDateTimeEdit* endTimeEdit_;
     QPushButton* commitChanges_;
     QPushButton* addElement_;
     QTreeWidget* elements_;
     AddSensorDialog* dlg_;
};
