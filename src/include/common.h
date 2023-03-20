#pragma once
#include "agent_event.h"

/// @brief флаги данных
#define BOOL_TYPE                  ( 1 << 10 )
#define FLOAT_TYPE                 ( 1 << 11 )
#define UIN32_TYPE                 ( 1 << 12 )
#define INT32_TYPE                 ( 1 << 13 )
#define BOOL_VALUE(val)            ( ( val & ( 1 << 15 ) ) >> 15 )
#define MSEC(val)                  ( val & 0x3FF )

/// @brief проверить установку флага
#define FLAG_ON(val1, val2)        ( val1 & val2 )

/// @brief преобразовать значение в QByteArray
#define TO_QBYTEARRAY(val)         QByteArray( ( const char* )&val, sizeof( val ) )

