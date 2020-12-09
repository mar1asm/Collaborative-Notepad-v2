#include "clientwindow.h"
#include <QApplication>

int main ( int argc, char *argv[] ) {
  QApplication a ( argc, argv );
  ClientWindow w;
  if ( w.setConnectionData ( argc, argv ) )
    return -1;
  w.show ( );
  return a.exec ( );
}
