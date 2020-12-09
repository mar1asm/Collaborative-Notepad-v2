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
  return 0;
}

void FilesServer::stopServer ( ) {}

void FilesServer::waitingForClients ( ) {
  if ( listen ( socketDescriptor, 10 ) == -1 )
    return;
  for ( int i = 0; i < threadCount; i++ )
    threadCreate ( i );
  incomingConnection ( socketDescriptor );
}

void FilesServer::threadCreate ( int i ) {
  threadPool.push_back ( std::thread ( &FilesServer::treat, this, i ) );
  emit logMessage ( QStringLiteral ( "ceva orice" ) );
  return; /* threadul principal returneaza */
}

void *FilesServer::treat ( int arg ) {
  int client;

  struct sockaddr_in from;
  bzero ( &from, sizeof ( from ) );
  printf ( "[thread]- %d - pornit...\n", arg );
  fflush ( stdout );

  for ( ;; ) {
    socklen_t length = sizeof ( from );
    pthread_mutex_lock ( &lock );
    // printf ( "Thread %d trezit\n", ( int ) arg );
    if ( ( client = accept ( socketDescriptor, ( struct sockaddr * ) &from,
                 &length ) ) < 0 ) {
      perror ( "[thread]Eroare la accept().\n" );
    }
    procesare_mesaje_client ( client, arg );
    pthread_mutex_unlock ( &lock );

    emit logMessage (
    QStringLiteral ( "am rezolvat acest client" ) ); // procesarea cererii
    /* am terminat cu acest client, inchidem conexiunea */
    // close ( client );
  }
}

void FilesServer::procesare_mesaje_client ( int client, int idThread ) {
  int nr; // mesajul primit de trimis la client

  if ( read ( client, &nr, sizeof ( int ) ) <= 0 ) {
    printf ( "[Thread %d]\n", idThread );
    perror ( "Eroare la read() de la client.\n" );
  }

  printf ( "[Thread %d]Mesajul a fost receptionat...%d\n", idThread, nr );
}

int FilesServer::incomingConnection ( int socketDescriptor ) {
  for ( ;; ) {
    printf ( "[server]Asteptam la portul %d...\n", PORT );
    pause ( );
  }
}
