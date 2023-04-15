qt_add_library(srv_plg_view_data
        SHARED
        view_data.cpp
        db_wrapper_view_data.cpp
        )

INCLUDE_DIRECTORIES( "../../include" . )

target_link_libraries( srv_plg_view_data PUBLIC Qt::Core Qt::Sql view_cmd db_wrapper )
