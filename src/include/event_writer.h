#pragma once
#include "db_wrapper.h"
#include "agent_event.h"
#include "limits"


enum AddToWriteEventResult
{
     AtwerOk,
     AtwerCountWarning,
     AtwerCountError,
     AtwerDataTypeError
};


class EventWriter: public QThread
{
     Q_OBJECT
public:
     EventWriter( quint64 warnCount = std::numeric_limits< quint64 >::max(), quint64 errorCount = std::numeric_limits< quint64 >::max() );
     AddToWriteEventResult addToWriteEvent( quint32 clientId, QList< QPointer< AgentEvent > >&& events );
     void run() override;
     bool isInited() const;
     quint64 currentQueueCount();

public slots:
     void stop();

private:
     struct DataToWrite
     {
          QVariantList clientId;
          QVariantList sensorId;
          QVariantList time;
          QVariantList msecs;
          QVariantList data;
     };

private:
     /// @return если false параметры не были успешно записаны, иначе true
     /// @attention внутри происходит вызов addTypeIfNeed(см. attention)
     bool writeEvents( const QQueue< QPair< quint32, QList< QPointer< AgentEvent > > > >& events );

     /// @brief добавляет новый тип данных, если его не существовало ранее
     /// @return true, если тип был добавлен или добавление не требуется. Иначе false.
     /// @param sqlConnection должен быть открыт на запись и находиться в состоянии начатой транзакции, если требуется
     /// @attention захватывает mutex existTypesMutex_ на чтение и, если требуется, запись
     /// @attention не проверяет соответствие типов. Т.е. метод проверяет только факт существования типа type, но не соответствия
     /// type ранее известному типу
     inline bool addTypeIfNeed( quint32 clientId, quint32 sensorId, const AgentEventType& type, SqlWrapperProtection* sqlConnection );


     /// @brief проверяет соответствие типов событий уже известным типам
     /// @return true если все типы событий соответствуют ранее заданным типам или ранее типы не были заданы. false если хотя бы одно событие не соответствует
     /// @attention захватывает mutex existTypesMutex_ на чтение
     bool validateEventsTypes( quint32 clientId, const QList<QPointer<AgentEvent>>& events );


     /// @brief проверяет, что тип type соответствует ранее известному типу.
     /// @return true - если type соотвуетствует типу для clientId + пары clientId + sensorId, или такого типа ранее не существовало.
     /// Если type не соответствует ранее существовавшему типу, возвращается false. Добавление такого события опасно для базы.
     bool validateTypeIfExist( quint32 clientId, quint32 sensorId, const AgentEventType& type );

     inline void prepareEvent( quint32 clientId, const AgentEvent& event, DataToWrite& data );
private:
     const quint64 warnCount_;
     const quint64 errorCount_;


     QMutex writeQueueMutex_;
     quint64 eventCount_ = 0;
     QWaitCondition queueCondition_;
     QQueue< QPair< quint32,  QList< QPointer< AgentEvent > > > > writeQueue_;

     QReadWriteLock existTypesMutex_;
     /// @param quint64 = clientId << 32 | sensorId
     QHash< quint64, AgentEventType > existTypes_;

     bool work_ = true;
     bool inited_ = false;

private:
     /// переменные для записи в них значений события(или константные значения) для последующий записи в базу
     /// могут вызываться только из потока, который происходит в методе run
     const bool cacheTrueVal = true;
     const bool cacheFalseVal = false;
     quint32 cacheQuint;
     qint32 cacheQint;
     float cacheFloat;
};

extern EventWriter* eventWriter;