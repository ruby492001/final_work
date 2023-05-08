#pragma once

#include <QtCore>
#include <QtWidgets>
#include <QSqlDatabase>

#include "view_cmd.h"


class ViewPluginItf: public QObject
{
public:
     /// @return имя плагина, будет отображено в программе просмотра
     virtual QString name() const= 0;

     /// @return идентификатор плагина
     virtual quint32 id() const = 0;

     /// @return изображение плагина для отображения в списке доступных плагинов
     virtual QIcon icon() const = 0;

     /// @return приоритет в списке доступных плагинов
     virtual quint32 priority() const = 0;

     /// @brief инициализация плагина
     virtual void init() = 0;

     /// @return виджет плагина, родителем которого является parent
     virtual QWidget* widget( QWidget* parent ) = 0;

public slots:
     /// @brief лот вызывается для обработки ответа от плагина программы сбора и хранения данных
     virtual void onNetResponse( const ViewCommand& cmd ) = 0;

signals:
     /// @brief сигнал посылается для уведомления программы просмотра о необходимости повторного запроса изображения плагина
     void sChangeIcon();

     /// @brief сигнал посылается для отправки запроса программе сбора и хранения данных
     void sSendNetRequest( const ViewCommand& cmd );
};

Q_DECLARE_INTERFACE( ViewPluginItf, "FINAL_WORK.ViewPluginItf/1.0" )