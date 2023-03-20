qt_add_library(event_writer
        SHARED
        ../../include/event_writer.h
        event_writer.cpp
        )

INCLUDE_DIRECTORIES( "../../include" )

target_link_libraries(event_writer PUBLIC Qt::Core Qt::Network agent_event)

