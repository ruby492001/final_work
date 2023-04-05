qt_add_library(view_plg_edit_sensor
        SHARED
        edit_sensor.cpp
        widget.cpp
        model.cpp
        )

INCLUDE_DIRECTORIES( "../../include" . )

target_link_libraries( view_plg_edit_sensor PUBLIC Qt::Core Qt::Widgets Qt::Sql view_cmd agent_event )

# Resources:
set(main_server_ui_resource_files
        "../../resource/images/edit_settings.png"
        "../../resource/images/save.png"
        "../../resource/images/rollback.png"
        )

qt_add_resources(view_plg_edit_sensor "view_plg_edit_sensor"
        "rollback"
        "save"
        PREFIX "/images"
        BASE "../../resource/images"
        FILES ${main_server_ui_resource_files}
        )