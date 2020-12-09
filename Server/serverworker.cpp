#include "serverworker.h"
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

ServerWorker::ServerWorker ( QObject *parent, int socketDescriptor )
    : QObject ( parent ) {
  bzero ( &fromSocket, sizeof ( fromSocket ) );
  socklen_t length = sizeof ( fromSocket );
  if ( ( this->client =
         accept ( socketDescriptor, ( struct sockaddr * ) &fromSocket,
              &length ) ) < 0 ) {
    perror ( "[thread]Eroare la accept().\n" );
  }
  close ( this->client );
  return;
  // receiveData ( );
}

void ServerWorker::receiveData ( ) {
  int size;
  QString msg;
  if ( read ( client, &size, sizeof ( int ) ) <= 0 ) {
    perror ( "Eroare la read() de la client.\n" );
  }
  if ( read ( client, &msg, size ) <= 0 ) {
    perror ( "Eroare la read() de la client.\n" );
  }
  emit logMessage ( QLatin1String ( "Received msg " ) + msg );
  return;
}
