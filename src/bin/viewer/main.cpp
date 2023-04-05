#include <QtCore>
#include <QtWidgets>
#include "view_connection_client.h"
#include "main_windows.h"




int main( int argc, char** argv )
{
     qRegisterMetaType< QList<ViewArgs> >();
     QApplication app( argc, argv );
     MainWindow t;
     t.show();
     return app.exec();
}
