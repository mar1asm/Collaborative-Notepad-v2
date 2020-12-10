#ifndef SERVERWORKER_H
#define SERVERWORKER_H
#include <QObject>
#include <QReadWriteLock>
#include <QThread>
#include <QVector>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <vector>
class ServerWorker : public QThread {
  Q_OBJECT
  Q_DISABLE_COPY ( ServerWorker )

public:
  ServerWorker ( int socketDescriptor );

private:
  void run ( ) override;
  int id;
  void procesare_mesaje_client ( );
  int socketDescriptor;
  void setare ( );
  pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
  struct sockaddr_in fromSocket;
  int client;
  // public slots:
  //  void disconnectFromClient ( );
  // private slots:
  // void receiveData ( );
signals:
  void resultReady ( const QString &msg );
  void logMessage ( const QString &msg );
};

#endif // SERVERWORKER_H
