#pragma once

#include <QtWidgets>

#include "edit_sensor_defs.h"


class EditSensorModel: public QAbstractItemModel
{
     Q_OBJECT
public:
     int columnCount( const QModelIndex &parent = QModelIndex() ) const override;
     QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;
     QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const override;
     QModelIndex parent( const QModelIndex& child ) const override;
     int rowCount( const QModelIndex& parent = QModelIndex() ) const override;
     QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;
     bool setData( const QModelIndex& index, const QVariant& value, int role = Qt::EditRole ) override;
     Qt::ItemFlags flags( const QModelIndex& index ) const override;

signals:
     void sDataChanged( const SensorParam& param );

public slots:
     void onSetSensorList( const QList< SensorParam >& sensors );

private:
     QList< SensorParam > sensors_;
};