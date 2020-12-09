#include "filesserver.h"
#include <QTimer>
#include <functional>

FilesServer::FilesServer ( QObject *parent ) {}

FilesServer::~FilesServer ( ) { // close all the threadz
  return;
}

int FilesServer::startServer ( ) {
  if ( ( socketDescriptor = socket ( AF_INET, SOCK_STREAM, 0 ) ) == -1 ) {
    emit logMessage ( QStringLiteral ( "Eroare la socket())" ) );
    perror ( "[server]Eroare la socket().\n" );
    return errno;
  }
  int on = 1;
  setsockopt ( socketDescriptor, SOL_SOCKET, SO_REUSEADDR, &on, sizeof ( on ) );
  bzero ( &server, sizeof ( server ) );
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl ( INADDR_ANY );
  server.sin_port = htons ( PORT );
  if ( bind ( socketDescriptor, ( struct sockaddr * ) &server,
          sizeof ( struct sockaddr ) ) == -1 ) {
    emit logMessage ( QStringLiteral ( "Eroare la bind()" ) );
    perror ( "[server]Eroare la bind().\n" );
    return errno;
  }
  if ( listen ( socketDescriptor, 10 ) == -1 ) {
    perror ( "Eroare la listen()\n" );
    return errno;
  }
  return 0;
}

void FilesServer::stopServer ( ) {}

int FilesServer::incomingConnection ( ) {
  for ( ;; ) {
    emit logMessage ( QStringLiteral ( "Asteptam la portul %1" ).arg ( PORT ) );
    pause ( );
  }
}
