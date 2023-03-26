qt_add_library(view_connection_client
        SHARED
        ../../include/view_connection_client.h
        view_connection_client.cpp
        )

INCLUDE_DIRECTORIES( "../../include" )

target_link_libraries(view_connection_client PUBLIC Qt::Core Qt::Network view_connection_trd)