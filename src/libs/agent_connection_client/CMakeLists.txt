qt_add_library(agent_connection_client
        SHARED
        ../../include/agent_connection_client.h
        agent_connection_client.cpp
        )

INCLUDE_DIRECTORIES( "../../include" )

target_link_libraries(agent_connection_client PUBLIC Qt::Core Qt::Network)

