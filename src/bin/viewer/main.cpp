#include <QtCore>
#include <QtWidgets>
#include <QTranslator>
#include "view_connection_client.h"
#include "main_windows.h"




int main( int argc, char** argv )
{
     qRegisterMetaType< QList<ViewArgs> >();
     QApplication app( argc, argv );
     QTranslator* translator = new QTranslator();
     if( translator->load( "../translation/qtbase_ru.qm ") )
     {
          app.installTranslator( translator );
     }
     MainWindow t;
     t.show();
     return app.exec();
}
