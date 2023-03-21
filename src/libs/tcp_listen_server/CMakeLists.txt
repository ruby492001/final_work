qt_add_library(tcp_listen_server
        SHARED
        ../../include/tcp_listen_server.h
        tcp_listen_server.cpp
        )

INCLUDE_DIRECTORIES( "../../include" )

target_link_libraries(tcp_listen_server PUBLIC Qt::Core Qt::Network)