#pragma once

#include <QtCore>
#include <QtWidgets>
#include <QSqlDatabase>

#include "view_cmd.h"


class ViewPluginItf: public QObject
{
public:
     virtual void name() = 0;
     virtual quint32 id() = 0;
     virtual QIcon icon() = 0;

     virtual void init() = 0;
     virtual void initDatabase( QSqlDatabase* db );
     virtual QWidget* widget() = 0;

public slots:
     void onNetResponse( const ViewCommand& cmd );

signals:
     void sChangeIcon();
     void sSendNetRequest( const ViewCommand& cmd );
};

Q_DECLARE_INTERFACE( ServicePluginItf, "FINAL_WORK.ViewPluginItf/1.0" )