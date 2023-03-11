qt_add_library(agent_event
        SHARED
        ../../include/agent_event.h
        agent_event.cpp
        )

INCLUDE_DIRECTORIES( "../../include" )

target_link_libraries(agent_event PUBLIC Qt::Core Qt::Network)

