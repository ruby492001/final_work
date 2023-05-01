#pragma once

#include <QtWidgets>
#include <QtGui>

#include "view_data_defs.h"
#include "add_sensor_model.h"


class AddSensorDialog: public QDialog
{
     Q_OBJECT
public:
     AddSensorDialog( QWidget* parent );
     QString resName();
     quint64 resId();
     QColor resColor();
     QString place();
     QString type();
     QString measure();


public slots:
     void onSetSensorList( const QList<SensorParam>& sensors );

private slots:
     void onSelectColorBtn();
     void onOkBtn();

private:
     QString resName_;
     quint64 resId_;
     QColor resColor_;
     QString place_;
     QString type_;
     QString measure_;
     AddSensorModel* model_;
     QTreeView* list_;
     QPushButton* selectColor_;
     QPushButton* okBtn_;
     QPushButton* cancelBtn_;
     bool colorSelected = false;
};