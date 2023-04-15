qt_add_library(qcustomplot
        SHARED
        ../../include/qcustomplot.h
        qcustomplot.cpp
        )

INCLUDE_DIRECTORIES( "../../include" )

target_link_libraries(qcustomplot PUBLIC Qt::Core Qt::Gui)