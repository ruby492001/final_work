#pragma once


#include <QtCore>

/// @brief размер uuid в байтах
#define UUID_SIZE 32


/// @brief запросы от агента к серверу
#define LOGIN_REQUEST_CMD                    0
#define GENERATE_AGENT_ID_REQUEST_CMD        1
#define SEND_DATA_REQUEST_CMD                2

/// @brief ответы от сервера агенту
#define RESPONSE_BASE_CMD                    ( 1 << 16 )
#define LOGIN_REQUEST_RESPONSE_CMD           ( RESPONSE_BASE_CMD + 1 )
#define GENERATE_AGENT_ID_RESPONSE_CMD       ( RESPONSE_BASE_CMD + 2 )
#define SEND_DATA_RESPONSE_CMD               ( RESPONSE_BASE_CMD + 3 )


/// @brief коды ошибок
#define NO_ERROR                             0
#define INTERNAL_ERROR                       1
#define INCORRECT_REQUEST_ERROR              2
#define TO_MANY_REQUESTS_ERROR               3
#define INCORRECT_LOGIN_ERROR                4
#define TO_MANY_REQUEST_WARNING              6
#define INCORRECT_DATA_TYPE_ERROR            7

/// @brief таймаут операций
#define TIMEOUT 20000


#pragma pack(push,1)


struct NetworkPackage
{
     qsizetype size;
     unsigned char data[ 1 ];
};


struct AgentProtoMain
{
     quint32 cmd;
};


struct ServerProtoMain
{
     quint32 cmd;
     bool ok;
     quint8 errorCode;
};


struct AgentConnectionRequestAgentId
{
     AgentProtoMain req;
     quint32 agentId;
     char uuid[ UUID_SIZE ];
};


struct AgentConnectionResponse
{
     ServerProtoMain res;
     quint32 agentId;
};


struct AgentSendDataRequest
{
     AgentProtoMain req;
     unsigned char data[1];
};


struct OneTimeRequest
{
     quint64 time;
     quint32 count;
     unsigned char data[1];
};


struct OneEventDataRequest
{
     // id датчика
     quint32 id;
     // [0 - 9] - msecs. Если его не существует, устанавливается 1000
     // [10] - тип bool
     // [11] - float
     // [12] - uint32
     // [13] - int32
     // [14] - reserved
     // [15] - bool, если тип bool
     quint16 flagsMsecs;
     // data может не быть, если тип bool
     unsigned char data[ 4 ];
};

#pragma pack(pop)
