#ifndef FILESSERVER_H
#define FILESSERVER_H
#include "serverworker.h"
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
  // void waitingForClients ( );

protected:
  int incomingConnections ( );

private:
  ~FilesServer ( );
  const int m_idealThreadCount;
  QVector< QThread * > m_availableThreads;
  QVector< int > m_threadsLoad;
  QVector< ServerWorker * > m_clients;
  struct sockaddr_in server;
  int serverSocketDescriptor;
  int *clientSocketDescriptor;
  QObject serverWindow;
public slots:
  void handleResults ( const QString &msg ) { emit logMessage ( msg ); };
signals:
  void logMessage ( const QString &msg );
  void stopAllClients ( );
};

#endif // FILESSERVER_H
