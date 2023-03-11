qt_add_library(cred_validator
        SHARED
        ../../include/cred_validator.h
        cred_validator.cpp
        )

INCLUDE_DIRECTORIES( "../../include" )

target_link_libraries(cred_validator PUBLIC Qt::Core Qt::Network)

