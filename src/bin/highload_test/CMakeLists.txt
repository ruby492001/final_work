qt_add_executable( highload_test
        main.cpp
        thread_wrapper.cpp
        )


INCLUDE_DIRECTORIES( ../../include )

target_link_libraries( highload_test PUBLIC Qt::Core agent_connection_client agent_event )


