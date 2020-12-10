#include "filesserver.h"
#include <QThread>
#include <QTimer>
#include <functional>

FilesServer::FilesServer ( QObject *parent )
    : m_idealThreadCount ( qMax ( QThread::idealThreadCount ( ), 1 ) ) {
  clientSocketDescriptor = new int[ 100 ];
  m_availableThreads.reserve ( m_idealThreadCount );
  m_threadsLoad.reserve ( m_idealThreadCount );
}

FilesServer::~FilesServer ( ) { // close all the threadz
  for ( QThread *singleThread : m_availableThreads ) {
    singleThread->quit ( );
    singleThread->wait ( );
  }
}

int FilesServer::startServer ( ) {
  if ( ( serverSocketDescriptor = socket ( AF_INET, SOCK_STREAM, 0 ) ) == -1 ) {
    emit logMessage ( QStringLiteral ( "Eroare la socket())" ) );
    perror ( "[server]Eroare la socket().\n" );
    return errno;
  }
  int on = 1;
  setsockopt ( serverSocketDescriptor, SOL_SOCKET, SO_REUSEADDR, &on,
           sizeof ( on ) );
  bzero ( &server, sizeof ( server ) );
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl ( INADDR_ANY );
  server.sin_port = htons ( PORT );
  if ( bind ( serverSocketDescriptor, ( struct sockaddr * ) &server,
          sizeof ( struct sockaddr ) ) == -1 ) {
    emit logMessage ( QStringLiteral ( "Eroare la bind()" ) );
    perror ( "[server]Eroare la bind().\n" );
    return errno;
  }
  if ( listen ( serverSocketDescriptor, 10 ) == -1 ) {
    perror ( "Eroare la listen()\n" );
    return errno;
  }
  incomingConnections ( );
}

void FilesServer::stopServer ( ) {}

int FilesServer::incomingConnections ( ) {
  while ( true ) {
    struct sockaddr_in fromSocket;
    socklen_t length = sizeof ( fromSocket );
    if ( ( int client =
           accept ( socketDescriptor, ( struct sockaddr * ) &fromSocket,
            &length ) ) < 0 ) {
      perror ( "[thread]Eroare la accept().\n" );
    }
    emit logMessage ( QStringLiteral ( "New client connected" ) );
    procesare_mesaje_client ( );
    pthread_mutex_unlock ( &lock );
    emit logMessage (
    QStringLiteral ( "am rezolvat acest client" ) ); // procesarea cererii
    /* am terminat cu acest client, inchidem conexiunea */
    close ( client );
    ServerWorker *worker = new ServerWorker ( );
    int threadIdx = m_availableThreads.size ( );
    if ( threadIdx < m_idealThreadCount ) { // we can add a new thread
      m_availableThreads.append ( new QThread ( this ) );
      m_threadsLoad.append ( 1 );
      m_availableThreads.last ( )->start ( );
    } else {
      // find the thread with the least amount of clients and use it
      threadIdx = std::distance ( m_threadsLoad.cbegin ( ),
                  std::min_element ( m_threadsLoad.cbegin ( ),
                             m_threadsLoad.cend ( ) ) );
      ++m_threadsLoad[ threadIdx ];
    }
    worker->moveToThread ( m_availableThreads.at ( threadIdx ) );
    connect ( m_availableThreads.at ( threadIdx ), &ServerWorker::finished,
          worker, &QObject::deleteLater );
    connect ( worker, &ServerWorker::resultReady, this,
          &FilesServer::handleResults );
    connect ( worker, &ServerWorker::logMessage, this,
          &FilesServer::logMessage );
    m_clients.append ( worker );
    printf ( "[server]Asteptam la portul %d...\n", PORT );
    worker->start ( );
    //  connect (
    //      worker, &ServerWorker::disconnectedFromClient, this,
    //      std::bind ( &ChatServer::userDisconnected, this, worker, threadIdx )
    //      );
    //  connect ( worker, &ServerWorker::error, this,
    //        std::bind ( &ChatServer::userError, this, worker ) );
    //  connect ( worker, &ServerWorker::dataReceived, this,
    //        std::bind ( &ChatServer::dataReceived, this, worker,
    //            std::placeholders::_1 ) );
    //  connect ( this, &ChatServer::stopAllClients, worker,
    //        &ServerWorker::disconnectFromClient );
  }
}
