#include "app.h"
#include "view_cmd.h"


int main( int argc, char** argv )
{
     qRegisterMetaType< QList<ViewArgs> >();

     ServerApp app( argc, argv );
     return app.start();
}
