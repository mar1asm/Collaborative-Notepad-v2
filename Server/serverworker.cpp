#include "serverworker.h"

ServerWorker::ServerWorker ( QObject *parent ) : QObject ( parent ) {
  socklen_t length = sizeof ( fromSocket );
  pthread_mutex_lock ( &lock );
  if ( ( client = accept ( socketDescriptor, ( struct sockaddr * ) &from,
               &length ) ) < 0 ) {
    perror ( "[thread]Eroare la accept().\n" );
  }
}
