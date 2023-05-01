#pragma once


#include "QtCore"
#include "event_writer.h"


class Tracer: public QObject
{
     Q_OBJECT
public:
     Tracer( EventWriter* writer, const QString& path, quint32 sec );

private slots:
     void onTimerShot();

private:
     QTimer timer_;
     QString path_;
     quint64 startTime_;
     EventWriter* writer_;
};
