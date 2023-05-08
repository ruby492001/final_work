#pragma once

#include "viewplg_itf.h"
#include "widget.h"



class ViewDataView: public ViewPluginItf
{
     Q_OBJECT
     Q_INTERFACES( ViewPluginItf )
     Q_PLUGIN_METADATA( IID "FINAL_WORK.ViewPluginItf/1.0" )
public:
     QString name() const override;
     quint32 id() const override;
     QIcon icon() const override;
     quint32 priority() const override;
     void init() override;
     QWidget* widget( QWidget* parent ) override;

public slots:
     void onNetResponse( const ViewCommand& cmd ) override;

signals:
     void sChangeIcon();
     void sSendNetRequest( const ViewCommand& cmd );

private slots:
     void onWidgetRequestSensor();
     void onWidgetRequestData( const QList< qint64 >& data );

/*private*/ signals:
     void sWidgetResponseSensor( const QList<SensorParam>& sensors );
     void sWidgetResponseData( const QList< DataFormat >& data );
private:
     void parseResponseSensor( const ViewCommand& cmd );
     void parseResponseData( const ViewCommand& cmd );

private:
     ViewDataWidget* wgt_;
};