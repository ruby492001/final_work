#pragma once


#include <QtCore>
#include <QtNetwork>
#include "view_cmd.h"
#include "srvplg_itf.h"



class ViewTrdWrapper: public QObject
{
     Q_OBJECT
public:
     ViewTrdWrapper( quintptr socket );
     virtual ~ViewTrdWrapper();

public slots:
     /// @brief вызывать только после пермещения в отдельный поток и только через сигнал
     void onStart();

     void onStop();

signals:
     void sDisconnected();

private:
     void handleReceivedData();

private slots:
     void onReadyRead();
     void onSocketError( QAbstractSocket::SocketError error );


/// сигналы и слоты для обработки в srv_plg
private slots:
     void onSendViewResponse( const ViewCommand& cmd );

/*private*/ signals:
     void sViewRequest( const ViewCommand& cmd );

/// END(сигналы и слоты для обработки в srv_plg)

/// @brief слоты для отладочного вывода обмена команадами с просмоторщиком
private slots:
     void onSendViewResponseDbg( const ViewCommand& cmd );
     void onViewRequestDbg( const ViewCommand& cmd );

private:
     void loadPlugins();

private:
     QTcpSocket* socket_ = nullptr;
     QByteArray pendingData_;
     QQueue<ViewCommand> requests_;
     quintptr sockHandle_;
     QList< ServicePluginItf* > plugins_;
};












class ViewTrd: public QObject
{
     Q_OBJECT
public:
     ViewTrd( quintptr socket );
     virtual ~ViewTrd();
     void startTrd();
     void onStop();

signals:
     void sDisconnected( ViewTrd* ptr );

signals:
     void sStop( QPrivateSignal );
     void sSetSocketDesc( quintptr socket, QPrivateSignal );

private slots:
     void onDisconnect();

private:
     QThread trd_;
     ViewTrdWrapper* wrapper_ = nullptr;

};