qt_add_executable(viewer_bin
        main.cpp
        net_cfg_dialog.cpp
        main_windows.cpp
        )


INCLUDE_DIRECTORIES ( ../../include . )

target_link_libraries(viewer_bin PUBLIC Qt::Core Qt::Widgets Qt::Sql Qt::Network view_cmd view_connection_client )


