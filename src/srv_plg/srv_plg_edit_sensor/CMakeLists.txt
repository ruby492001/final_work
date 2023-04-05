qt_add_library(srv_plg_edit_sensor
        SHARED
        edit_sensor.cpp
        db_wrapper_edit_sensor.cpp
        )

INCLUDE_DIRECTORIES( "../../include" . )

target_link_libraries( srv_plg_edit_sensor PUBLIC Qt::Core Qt::Sql view_cmd db_wrapper )
