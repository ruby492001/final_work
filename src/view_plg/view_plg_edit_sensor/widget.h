#pragma once


#include <QtWidgets>

#include "view_cmd.h"
#include "model.h"


class EditSensorConfigWidget: public QWidget
{
     Q_OBJECT
public:
     EditSensorConfigWidget( QWidget* parent );

public slots:
     void onUpdateTable( const QList< SensorParam >& elements );

signals:
     void sSendChanges( const QList< SensorParam >& elements );
     void sUpdateTableRequest();

private slots:
     void onNewChanges( const SensorParam& element );
     void onCommitChanges();
     void onRollback();

private:
     EditSensorModel* model_;
     QTreeView* treeView_;
     QPushButton* updateBtn_;
     QPushButton* rollbackBtn_;
     QList< SensorParam > localChanges_;
};