qt_add_library(view_connection_server
        SHARED
        ../../include/view_connection_server.h
        view_connection_server.cpp
        )

INCLUDE_DIRECTORIES( "../../include" )

target_link_libraries(view_connection_server PUBLIC Qt::Core Qt::Network view_connection_trd)