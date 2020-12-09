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

void FilesServer::startServer ( ) {}

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
