#pragma once

#include <QtCore>
#include <QTcpSocket>
#include "agent_event.h"
#include <limits>

#define INVALID_AGENT_ID quint32( -1 )


enum AgentConnectionClientStates
{
     AgentConnectionClientUndefined,
     AgentConnectionClientConnected,
     AgentConnectionClientActive,
     AgentConnectionClientError,
     AgentConnectionClientProtocolError
};


class AgentConnectionClient: public QObject
{
     Q_OBJECT
public:
     AgentConnectionClient( const QString& ipAddress, const quint16 port );
     bool setAgentId( quint32 agentId = INVALID_AGENT_ID, const QString& uuid = QString() );
     AgentConnectionClientStates connectedState();
     bool onFlushEvents();

public slots:
     void onConnect();
     void onAddEvent( const AgentEvent& event );
     void onStartSending(){};
     void onStopSending(){};

signals:
     void sGeneratedAgentId( quint32 newAgentId, const QString& uuid );
     void sConnectionLost();


private:
     bool onNotifyServer();
     bool onRequestAgentId();
     QByteArray prepareSomeEvents( const QList< AgentEvent >& list );
     QByteArray prepareEvent( const AgentEvent& event );
     QString generateUuid();
     bool sendEvents();
     QByteArray packData( const QByteArray& data );
     QByteArray packData( const unsigned char* data, quint32 dataSize );

private slots:
     void onStateChanged( QAbstractSocket::SocketState newState_ );
     void onSocketError( QAbstractSocket::SocketError socketError );

private:
     QHostAddress addr_;
     quint16 port_;
     QTcpSocket sock_;
     quint32 agentId_ = INVALID_AGENT_ID;
     QString uuid_;
     QList< AgentEvent > queue_;
     AgentConnectionClientStates state_ = AgentConnectionClientUndefined;
     QTimer* timer = nullptr;
     quint32 timeouts_ = 20000;
};