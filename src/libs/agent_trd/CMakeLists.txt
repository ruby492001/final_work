qt_add_library(agent_trd
        SHARED
        ../../include/agent_trd.h
        agent_trd.cpp
        )

INCLUDE_DIRECTORIES("../../include")

target_link_libraries(agent_trd PUBLIC Qt::Core Qt::Network cred_validator db_wrapper event_writer)

