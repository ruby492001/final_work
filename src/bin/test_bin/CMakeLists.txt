qt_add_executable(test_bin
        main.cpp
        )


INCLUDE_DIRECTORIES (../../include )

target_link_libraries(test_bin PUBLIC Qt::Core Qt::Widgets Qt::Network Qt::WebSockets Qt::Sql )


