qt_add_library(agent_connection_server
        SHARED
        ../../include/agent_connection_server.h
        agent_connection_server.cpp
        )

INCLUDE_DIRECTORIES("../../include")

target_link_libraries(agent_connection_server PUBLIC Qt::Core Qt::Network)

