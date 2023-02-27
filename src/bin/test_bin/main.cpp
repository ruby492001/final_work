#include <QtCore>
#include <QtWidgets>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "test_lib.h"

int main( int argc, char* argv[] )
{
     QApplication app( argc, argv );
     QSqlDatabase db = QSqlDatabase::addDatabase ("QSQLITE", "testDB");
     db.setDatabaseName("test.db");
     qDebug() << db.open();
     QSqlQuery q( db );
     q.exec( "CREATE TABLE test(INTEGER test_);" );
     TestLib t;
     QWidget wgt;
     wgt.setWindowTitle( t.getString() );
     wgt.show();
     app.exec();
     return 0;
}
