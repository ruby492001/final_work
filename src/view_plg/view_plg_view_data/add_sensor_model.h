#pragma once

#include <QtWidgets>

#include "view_data_defs.h"


class AddSensorModel: public QAbstractItemModel
{
     Q_OBJECT
public:
     int columnCount( const QModelIndex &parent = QModelIndex() ) const override;
     QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;
     QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const override;
     QModelIndex parent( const QModelIndex& child ) const override;
     int rowCount( const QModelIndex& parent = QModelIndex() ) const override;
     QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;
     Qt::ItemFlags flags( const QModelIndex& index ) const override;

public slots:
     void onSetSensorList( const QList< SensorParam >& sensors );

private:
     QList< SensorParam > sensors_;
};