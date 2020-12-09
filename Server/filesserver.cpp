#include "filesserver.h"
#include "serverworker.h"
#include <QThread>
#include <QTimer>
#include <functional>

FilesServer::FilesServer ( QObject *parent )
    : idealThreadCount ( qMax ( QThread::idealThreadCount ( ), 1 ) ) {
  availableThreads.reserve ( idealThreadCount );
  threadsLoad.reserve ( idealThreadCount );
}

FilesServer::~FilesServer ( ) { // close all the threadz
  for ( QThread *singleThread : availableThreads ) {
    singleThread->quit ( );
    singleThread->wait ( );
  }
}

int FilesServer::startServer ( ) {
  if ( ( socketDescriptor = socket ( AF_INET, SOCK_STREAM, 0 ) ) == -1 ) {
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
    perror ( "[server]Eroare la bind().\n" );
    return errno;
  }
  return 0;
}

void FilesServer::stopServer ( ) {}

int FilesServer::incomingConnection ( int socketDescriptor ) {
  ServerWorker *worker = new ServerWorker;
  int threadIdx = availableThreads.size ( );
  if ( threadIdx < idealThreadCount ) {
    availableThreads.append ( new QThread ( this ) );
    threadsLoad.append ( 1 );
    availableThreads.last ( )->start ( );
  } else {
    threadIdx = std::distance (
        threadsLoad.cbegin ( ),
        std::min_element ( threadsLoad.cbegin ( ), threadsLoad.cend ( ) ) );
    ++threadsLoad[ threadIdx ];
  }
  worker->moveToThread ( availableThreads.at ( threadIdx ) );
  clients.append ( worker );
  emit logMessage ( QStringLiteral ( "New client Connected" ) );

  return 0;
}
