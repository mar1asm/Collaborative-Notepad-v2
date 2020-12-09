#ifndef FILESSERVER_H
#define FILESSERVER_H
#include <QObject>
#include <QReadWriteLock>
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
class ServerWorker;
class QThread;

class FilesServer : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY ( FilesServer )
  static const int PORT = 2001;

public:
  explicit FilesServer ( QObject *parent = nullptr );
  int startServer ( );
  void stopServer ( );
  void waitingForClients ( );
  pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

protected:
  int incomingConnection ( int socketDescriptor );
  void threadCreate ( int );

private:
  ~FilesServer ( );
  void procesare_mesaje_client ( int client, int idThread );
  const int threadCount = std::thread::hardware_concurrency ( );
  struct sockaddr_in server;
  int socketDescriptor;
  std::vector< std::thread > threadPool;
  void *treat ( int );
  QObject serverWindow;
signals:
  void logMessage ( const QString &msg );
  void stopAllClients ( );
};

#endif // FILESSERVER_H
