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

  sendRequest ( { "username", ( char * ) this->username.data ( ) } );
  spawnListeningThread ( );
  return 0;
}

int ClientMain::threadCallback ( ) {
  int error = 0;
  while ( ! error ) {
    error = listen ( );
  }
}

void ClientMain::spawnListeningThread ( ) {
  listeningThread = new std::thread ( &ClientMain::threadCallback, this );
  listeningThread->detach ( );
}

int ClientMain::listen ( ) {
  int length;
  int closed;
  if ( closed =
       ( read ( socketDescriptor, &length, sizeof ( length ) ) ) < 0 ) {
    perror ( "[client]Eroare la read() de la server.\n" );
    return errno;
  } else {
    if ( closed == 0 )
      return -1;
  }
  char *msg;
  if ( read ( socketDescriptor, msg, length ) < 0 ) {
    perror ( "[client]Eroare la read() de la server.\n" );
    return errno;
  }
}

void ClientMain::sendRequest ( std::initializer_list< char * > msgs ) {
  // nu credeam ca o sa ajung sa folosesc asta vreodata
  for ( char *msg : msgs ) {
    int requestLength = strlen ( msg );
    if ( write ( socketDescriptor, &requestLength, sizeof ( int ) ) <= 0 ) {
      perror ( "[client]Eroare la write() spre server.\n" );
    }
    if ( write ( socketDescriptor, msg, requestLength ) <= 0 ) {
      perror ( "[client]Eroare la write() spre server.\n" );
    }
  }
}

void ClientMain::setUsername ( std::string username ) {
  this->username = username;
}
