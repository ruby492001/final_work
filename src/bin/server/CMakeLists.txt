qt_add_executable(server_bin
        main.cpp
        app.cpp
        )


INCLUDE_DIRECTORIES ( ../../include . )

target_link_libraries(server_bin PUBLIC Qt::Core agent_connection_server cred_validator db_wrapper event_writer
        view_connection_trd view_connection_server view_cmd )


