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
  char temp[] =
      "username"; // c++ doesnt allow conversion from string literal to char
  sendRequest ( 2, temp, ( char * ) this->username.data ( ) );
  return 0;
}

void ClientMain::sendRequest ( std::initializer_list< char * > msgs ) {
  // nu credeam ca o sa ajung sa folosesc asta vreodata
  for ( char *msg : msgs ) {
    int requestLength = strlen ( ms )
  }

  int requestLength = strlen ( requestType );
  int msgLength = strlen ( msg );
  if ( write ( socketDescriptor, &length, sizeof ( int ) ) <= 0 ) {
    perror ( "[client]Eroare la write() spre server.\n" );
  }
  if ( write ( socketDescriptor, msg, length ) <= 0 ) {
    perror ( "[client]Eroare la write() spre server.\n" );
  }
}

void ClientMain::setUsername ( std::string username ) {
  this->username = username;
}
