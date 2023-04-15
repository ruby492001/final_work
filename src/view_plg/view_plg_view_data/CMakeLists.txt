qt_add_library(view_plg_view_data
        SHARED
        view_data.cpp
        widget.cpp
        add_sensor_dialog.cpp
        add_sensor_model.cpp
        )

INCLUDE_DIRECTORIES( "../../include" . )

target_link_libraries( view_plg_view_data PUBLIC Qt::Core Qt::Widgets Qt::Sql view_cmd agent_event qcustomplot )

# Resources:
set(main_server_ui_resource_files
        "../../resource/images/graph.png"
        )

qt_add_resources(view_plg_edit_sensor "graph"
        PREFIX "/images"
        BASE "../../resource/images"
        FILES ${main_server_ui_resource_files}
        )