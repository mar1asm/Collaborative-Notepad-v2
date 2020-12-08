#include "filesserver.h"
#include <QThread>

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
