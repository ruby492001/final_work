#pragma once


#include <QtCore>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>



class SqlWrapper;
class SqlWrapperProtection;


class SqlConnectionManager: public QObject
{
     Q_OBJECT
public:
     SqlConnectionManager( const QString& pathToDb );

     bool removeConnections();

     static SqlWrapper* getReadOnlyConnection();

     /// @param critical - наивысший приоритет получения враппера. Задавать только в том случае, если операция НЕ затратная
     static SqlWrapper* getWriteConnection( bool critical = false );

     /// @brief вернуть соединение после использования
     static void returnConnection( SqlWrapper* wrapper );

private:
     static QMutex mutex_;

     static QWaitCondition condWrite_;
     static SqlWrapper* writeConnection_;
     static bool writeConnectionIsFree_;
     static quint32 criticalWriteRequestCount_;

     static QList< SqlWrapper* > activeReadConnection_;
     static QList< SqlWrapper* > notActiveReadConnection_;
     static QString pathToDb_;
     static quint32 connectionNumber_;

     static bool inited_;
};


class SqlWrapperProtection: public QObject
{
     Q_OBJECT
public:
     SqlWrapperProtection( SqlWrapper* wrapper );
     ~SqlWrapperProtection();
     SqlWrapper* operator *();
     SqlWrapper* operator->();

private:
     SqlWrapper* wrapper_;
};


class SqlWrapper: QObject
{
     Q_OBJECT
public:
     SqlWrapper( const QString& pathToDb, quint32 connectionNumber, bool readOnly = true );
     ~SqlWrapper();
     bool initDb();

     /// для работы с транзакциями
     bool beginTransaction();
     bool commitTransaction();
     bool rollbackTransaction();
     bool isTransactionStarted() const;

     /// для работы с кредами клиентов
     bool addCreds( quint32 clientId, const QByteArray& uuid );
     QList< QPair< quint32, QByteArray > > existClient();

     bool writeSomeEvents( const QVariantList& clientId, const QVariantList& sensorId, const QVariantList& time, const QVariantList& msec, const QVariantList& data );

     /// для работы с типами данных
     QList< QPair< quint64, quint8 > > registeredDataTypes();
     bool registerNewDataType( quint64 id, quint8 dataType );
     QSqlDatabase* db();

private:
     QSqlDatabase db_;
     bool readOnly_ ;
     bool transactionStarted_ = false;
};