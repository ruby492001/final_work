#include <QtCore>
#include "agent_connection_client.h"

class LoadThread: public QObject
{
     Q_OBJECT
public:
     LoadThread( quint64 eventCount, quint32 num );

public slots:
     void onSend();

private:
     QList<AgentEvent> events_;
     AgentConnectionClient* client_ = nullptr;
     quint32 num_;
};


class LoadThreadWrapper: public QObject
{
     Q_OBJECT
public:
     LoadThreadWrapper( quint64 threadCount, quint64 eventCount );

private:
     QTimer timer;
     QList< LoadThread* > workers_;
     QList< QThread* > threads_;
};