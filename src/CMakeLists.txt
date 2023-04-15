cmake_minimum_required(VERSION 3.16)

project(log_collector VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_PREFIX_PATH "/opt/qt6")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Woverloaded-virtual -Werror")


find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Network Sql WebSockets)
qt_standard_project_setup()

set( RESULT_OUTPUT ${CMAKE_SOURCE_DIR}/../result )

set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${RESULT_OUTPUT}/lib )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${RESULT_OUTPUT}/bin )

# добавляем сюда проекты, которые требуется собирать
add_subdirectory( libs )
add_subdirectory( external_libs )
add_subdirectory( bin )
add_subdirectory( view_plg )
add_subdirectory( srv_plg )