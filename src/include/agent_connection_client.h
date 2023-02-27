#pragma once

#include <QtCore>
#include <QTcpSocket>
#include "age


class AgentConnectionClient: public QObject
{
     Q_OBJECT
public:
     AgentConnectionClient( const QString& ipAddress, const quint16 port );



private:
     QTcpSocket sock_;
     quint16 agentId_ = 0;

};