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
#include <unistd.h>
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

protected:
  int incomingConnection ( int socketDescriptor );

private:
  ~FilesServer ( );
  const int idealThreadCount;
  QVector< QThread * > availableThreads;
  QVector< int > threadsLoad;
  QVector< ServerWorker * > clients;
  struct sockaddr_in server;
  int socketDescriptor;
private slots:
  void userDisconnected ( ServerWorker *sender, int threadIdx );
signals:
  void logMessage ( const QString &msg );
  void stopAllClients ( );
};

#endif // FILESSERVER_H
