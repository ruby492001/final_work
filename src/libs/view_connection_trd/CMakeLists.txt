qt_add_library(view_connection_trd
        SHARED
        ../../include/view_connection_trd.h
        view_connection_trd.cpp
        )

INCLUDE_DIRECTORIES( "../../include" )

target_link_libraries(view_connection_trd PUBLIC Qt::Core Qt::Network view_cmd)