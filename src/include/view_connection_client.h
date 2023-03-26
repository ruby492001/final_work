#pragma once


#include <QtCore>
#include <QtNetwork>

#include "view_cmd.h"


class ViewConnectionClient: public QObject
{
     Q_OBJECT
public:
     ViewConnectionClient( const QString& address, quint16 port );
     virtual ~ViewConnectionClient();


public slots:
     void onStop();
     void onSendCommand( const ViewCommand& cmd );
     void onConnect();

signals:
     void sConnected();
     void sResponseReceived( const ViewCommand& cmd );
     void sConnectionError();

private slots:
     void onDataAvailable();
     void onConnectionLost();

private:
     QTcpSocket* sock_;
     QByteArray pendingData_;

     QString addr_;
     quint16 port_;
};
