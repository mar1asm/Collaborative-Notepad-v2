#include "serverworker.h"

ServerWorker::ServerWorker ( int socketDescriptor ) {
  bzero ( &fromSocket, sizeof ( fromSocket ) );
}

void ServerWorker::run ( ) {
  QString result = "e ok";
  setare ( );
  emit resultReady ( result );
}

void ServerWorker::procesare_mesaje_client ( ) {
  int nr; // mesajul primit de trimis la client

  if ( read ( client, &nr, sizeof ( int ) ) <= 0 ) {
    emit logMessage ( QStringLiteral ( "Eroare la read() de la client." ) );
    perror ( "Eroare la read() de la client.\n" );
  }
  emit logMessage ( QStringLiteral ( "Mesajul %1 a fost primit" ).arg ( nr ) );
}
