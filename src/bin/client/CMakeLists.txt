qt_add_executable( client_bin
        main.cpp
        )


INCLUDE_DIRECTORIES( ../../include )

target_link_libraries( client_bin PUBLIC Qt::Core agent_connection_client agent_event )


