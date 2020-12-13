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
#include <atomic>
#include <errno.h>
#include <initializer_list>
#include <iostream>
#include <mutex>
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

// todo: sterge bibliotecile inutile
class ServerWorker;
class QThread;

class serverMain : public QObject {
  static const int PORT = 2001;
  Q_DISABLE_COPY ( serverMain )
public:
  explicit serverMain ( QObject *parent = nullptr );
  int startServer ( );
  void stopServer ( );
  void spawnSpawningThread ( );

private:
  ~serverMain ( );
  QObject serverWindow;
  Q_OBJECT

  // communication
  struct sockaddr_in serverSocket;
  struct sockaddr_in fromSocket;
  int serverSocketDescriptor;
  int *clientSocketDescriptor;
  const std::unordered_map< std::string, int > requestsNumbers {
      { "username", 1 }, { "quit", -1 }, { "list", 2 },
      { "update", 3 },	 { "new", 4 },	 { "idle", 5 } };

  // workers

  std::mutex lock; // aici e problema
  int threadsCount;
  std::thread *threadPool;
  int threadCallback ( int clientId );
  int waitForClients ( );
  int processRequest ( int clientId );
  void spawnThreads ( );
  std::atomic< bool > safeShutdown;

  std::vector< QFile * > files;
  std::vector< std::string > fileNames;
  QString filesPath;

  // requests
  void setUsername ( int clientId );
  void clientDisconnected ( int clientId );
  void sendListOfFiles ( int clientId );
  void updateFile ( int clientId );
  void createFile ( int clientId );
  void disconnectClient ( int clientId );
  void sendMessage ( int clientSocketDescriptor,
             std::initializer_list< std::string > msgs,
             bool sendLength = true );

  // aux
  int getAvailable ( );
  bool *available;
  void getFiles ( );
  std::string *clientsUsernames;

public slots:
  void handleResults ( const QString &msg ) { emit logMessage ( msg ); };

signals:
  void logMessage ( const QString &msg );
  // void startListening ( );
  // void stopAllClients ( );
};

#endif // SERVERMAIN_H
