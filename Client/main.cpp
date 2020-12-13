#include "clientwindow.h"
#include <QApplication>

int main ( int argc, char *argv[] ) {

  QApplication a ( argc, argv );
  ClientWindow w;
  qRegisterMetaType< QVector< QPair< QString, int > > > ( );
  if ( w.setConnectionData ( ) )
    return -1;
  w.show ( );
  return a.exec ( );
}
