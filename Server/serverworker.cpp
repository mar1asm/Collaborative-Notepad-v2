#include "serverworker.h"

ServerWorker::ServerWorker ( QObject *parent ) : QObject ( parent ) {
  bzero ( &fromSocket, sizeof ( fromSocket ) );
}

void ServerWorker::setare ( ) {
  socklen_t length = sizeof ( fromSocket );
  pthread_mutex_lock ( &lock );
  if ( ( client = accept ( socketDescriptor, ( struct sockaddr * ) &fromSocket,
               &length ) ) < 0 ) {
    perror ( "[thread]Eroare la accept().\n" );
  }
  procesare_mesaje_client ( client );
  pthread_mutex_unlock ( &lock );
  emit logMessage (
      QStringLiteral ( "am rezolvat acest client" ) ); // procesarea cererii
  /* am terminat cu acest client, inchidem conexiunea */
  close ( client );
}

void ServerWorker::procesare_mesaje_client ( int client ) {
  int nr; // mesajul primit de trimis la client

  if ( read ( client, &nr, sizeof ( int ) ) <= 0 ) {
    emit logMessage ( QStringLiteral ( "Eroare la read() de la client." ) );
    perror ( "Eroare la read() de la client.\n" );
  }
  emit logMessage ( QStringLiteral ( "Mesajul %1 a fost primit" ).arg ( nr ) );
}
