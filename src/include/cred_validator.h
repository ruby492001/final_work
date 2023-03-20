#pragma once

#include <QtCore>

#include "db_wrapper.h"


class CredValidator: public QObject
{
     Q_OBJECT
public:
     static bool init();

     /// @refurn успешность работы функции
     /// @param validateCredsRes[Out] - результат проверки кредов, если функция отработала успешно. Иначе значение не изменяется
     static bool validateCreds( quint32 clientId, const QByteArray& uuid, bool& validateCredsRes );

     /// @refurn успешность работы функции
     /// @param newClientId[Out] - id нового клиента, если функция отработала успешно. Иначе значение не изменяется
     static bool createNewClient( const QByteArray& uuid, quint32& newClientId );

private:
     static QMutex mutex_;
     static QHash< quint32, QByteArray >  clients_;
     static quint32 maxClientId_;
     static bool inited_;
};