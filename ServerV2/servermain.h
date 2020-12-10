#ifndef SERVERMAIN_H
#define SERVERMAIN_H
#include <QObject>
#include <QReadWriteLock>
#include <QString>
#include <QTextStream>
#include <QVector>
#include <errno.h>
#include <iostream>
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

class serverMain : public QObject {
  static const int PORT = 2001;
  Q_OBJECT
  Q_DISABLE_COPY ( serverMain )
public:
  explicit serverMain ( QObject *parent = nullptr );
  int startServer ( );
  void spawnThread ( );
  void stopServer ( );

private:
  ~serverMain ( );
  struct sockaddr_in serverSocket;
  struct sockaddr_in fromSocket;
  struct file {
    std::string name;
    std::string *lines;
  };
  std::vector< file > files;
  std::thread *threadPool;
  int serverSocketDescriptor;
  int *clientSocketDescriptor;
  bool *available;
  int threadsCount;
  QObject serverWindow;
  int processRequest ( int clientId );
  int threadCallback ( int clientId );
  int waitForClients ( );
  // helpers
  int getAvailable ( );
public slots:
  void handleResults ( const QString &msg ) { emit logMessage ( msg ); };

signals:
  void logMessage ( const QString &msg );
  // void startListening ( );
  // void stopAllClients ( );
};

#endif // SERVERMAIN_H
