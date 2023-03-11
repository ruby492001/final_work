qt_add_library(db_wrapper
        SHARED
        ../../include/db_wrapper.h
        db_wrapper.cpp
        )

INCLUDE_DIRECTORIES( "../../include" )

target_link_libraries(db_wrapper PUBLIC Qt::Core Qt::Network)

