#ifndef SERVERMAIN_H
#define SERVERMAIN_H
#include <QDir>
#include <QDirIterator>
#include <QFile>
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
#include <unordered_map>
#include <vector>
class ServerWorker;
class QThread;

class serverMain : public QObject {
  static const int PORT = 2001;
  const std::unordered_map< std::string, int > requestsNumbers {
      { "username", 1 }, { "quit", -1 } };
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
  std::vector< QFile * > files;
  std::thread *threadPool;
  int serverSocketDescriptor;
  int *clientSocketDescriptor;
  std::string *clientsUsernames;
  void setUsername ( int clientId );
  bool *available;
  int threadsCount;
  QObject serverWindow;
  int processRequest ( int clientId );
  int threadCallback ( int clientId );
  int waitForClients ( );
  int getAvailable ( );
  QString filesPath;
  void getFiles ( );
  void clientDisconnected ( int clientId );

public slots:
  void handleResults ( const QString &msg ) { emit logMessage ( msg ); };

signals:
  void logMessage ( const QString &msg );
  // void startListening ( );
  // void stopAllClients ( );
};

#endif // SERVERMAIN_H
