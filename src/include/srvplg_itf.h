#include <QtCore>
#include "view_cmd.h"


class ServicePluginItf: public QObject
{
public:
     /// @return идентификатор плагина
     virtual quint32 pluginId() = 0;

     /// @brief инициализация плагина
     /// @return true, если плагин успешно инициализирован, false - иначе
     virtual bool init() = 0;

     /// @brief инициализация таблиц, требуемых для работы плагина
     /// @return true, если плагин успешно инициализировал таблицы, false - иначе
     virtual bool initDbTables() = 0;

public slots:
     /// @brief слот обработки запроса от плагина просмотра
     virtual void onNetRequest( const ViewCommand& cmd ) = 0;

signals:
     /// @brief сигнал посылается для отправки ответа плагину просмотра
     void sSendResponse( const ViewCommand& cmd );
};

Q_DECLARE_INTERFACE( ServicePluginItf, "FINAL_WORK.ServicePluginItf/1.0" )