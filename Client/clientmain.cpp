#include "clientmain.h"

ClientMain::ClientMain ( QObject *parent ) : QObject ( parent ) {}

int ClientMain::connectToServer ( char *address, int port ) {
  if ( ( socketDescriptor = socket ( AF_INET, SOCK_STREAM, 0 ) ) == -1 ) {
    perror ( "[client] Eroare la socket().\n" );
    return errno;
  }
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr ( address );
  server.sin_port = htons ( port );
  if ( ::connect ( socketDescriptor, ( struct sockaddr * ) &server,
           sizeof ( struct sockaddr ) ) == -1 ) {
    perror ( "[client]Eroare la connect().\n" );
    return errno;
  }
  trimiteMesaj ( );
  return 0;
}

void ClientMain::trimiteMesaj ( ) {
  int nr = 10;
  if ( write ( socketDescriptor, &nr, sizeof ( int ) ) <= 0 ) {
    perror ( "[client]Eroare la write() spre server.\n" );
  }
}
