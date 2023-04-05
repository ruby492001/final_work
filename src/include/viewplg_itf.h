#pragma once

#include <QtCore>
#include <QtWidgets>
#include <QSqlDatabase>

#include "view_cmd.h"


class ViewPluginItf: public QObject
{
public:
     virtual QString name() const= 0;
     virtual quint32 id() const = 0;
     virtual QIcon icon() const = 0;
     virtual quint32 priority() const = 0;

     virtual void init( QSqlDatabase* db ) = 0;
     virtual QWidget* widget( QWidget* parent ) = 0;

public slots:
     virtual void onNetResponse( const ViewCommand& cmd ) = 0;

signals:
     void sChangeIcon();
     void sSendNetRequest( const ViewCommand& cmd );
};

Q_DECLARE_INTERFACE( ViewPluginItf, "FINAL_WORK.ViewPluginItf/1.0" )