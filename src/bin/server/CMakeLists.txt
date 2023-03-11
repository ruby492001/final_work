qt_add_executable(server_bin
        main.cpp
        app.cpp
        )


INCLUDE_DIRECTORIES ( ../../include . )

target_link_libraries(server_bin PUBLIC Qt::Core agent_connection_server )


