#pragma once


#include <QtCore>
#include <QtNetwork>
#include "agent_event.h"


class ClientTrd;

class ClientTrdWrapper: public QObject
{
     Q_OBJECT
public:
     ClientTrdWrapper( QObject* par );
     ~ClientTrdWrapper();

public slots:
     /// @brief вызывать только после пермещения в отдельный поток и только через сигнал
     void onSetSocketDesc( quintptr nativeHandle );

     void onStop();

signals:
     void sDisconnected( ClientTrd* cliTrd );

private:
     void handleData();

     void handleRequests();

     void disconnect();

     void handleLoginRequest( const QByteArray& arr );

     void handleGenAgentIdRequest( const QByteArray& arr );

     void handleSendDataRequest( const QByteArray& arr );

     void handleIncorrectRequest();

     void returnError( quint32 errorCode );

private slots:
     void onReadyRead();
     void onSocketError( QAbstractSocket::SocketError error );

private:
     bool parseEvents( const QByteArray& arr, QList< std::shared_ptr<AgentEvent> >& res );

private:
     QTcpSocket* socket_;
     QByteArray pendingData_;
     QQueue<QByteArray> requests_;
     bool authorized_ = false;
     quint32 clientId_ = -1;
     QObject* par_ = nullptr;
};


class ClientTrd: public QObject
{
     Q_OBJECT
public:
     ClientTrd( quintptr socket );
     ~ClientTrd();
     ClientTrdWrapper* getWrapper();
     void startTrd();

signals:
     void sStop( QPrivateSignal );
     void sSetSocketDescriptor( quintptr nativeHandle );
private:
     QThread trd_;
     ClientTrdWrapper* wrapper_;
};