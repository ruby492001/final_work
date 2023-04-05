#pragma once

#include "viewplg_itf.h"
#include "widget.h"


class EditSensorView: public ViewPluginItf
{
     Q_OBJECT
     Q_INTERFACES( ViewPluginItf )
     Q_PLUGIN_METADATA( IID "FINAL_WORK.ViewPluginItf/1.0" )
public:
     QString name() const override;
     quint32 id() const override;
     QIcon icon() const override;
     void init( QSqlDatabase* db ) override;
     QWidget* widget( QWidget* parent ) override;
     quint32 priority() const override;

public slots:
     void onNetResponse( const ViewCommand& cmd ) override;

signals:
     void sSendNetRequest( const ViewCommand& cmd );
     void sChangeIcon();

///сигналы и слоты виджета
private slots:
     void onSendChanges( const QList< SensorParam >& elements );
     void onUpdateTableRequest();

/*private*/ signals:
     void sUpdateTable( const QList< SensorParam >& elements );

private:
     void onUpdateTableRes( const ViewCommand& cmd );

private:
     EditSensorConfigWidget* wgt_ = nullptr;
};