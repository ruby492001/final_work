qt_add_library(view_cmd
        SHARED
        ../../include/view_cmd.h
        view_cmd.cpp
        )

INCLUDE_DIRECTORIES( "../../include" )

target_link_libraries(view_cmd PUBLIC Qt::Core Qt::Network)