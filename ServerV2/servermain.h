#ifndef SERVERMAIN_H
#define SERVERMAIN_H
#include "servecommunication.h"
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
      { "username", 1 }, { "quit", -1 },	{ "list", 2 },
      { "file", 3 },	 { "update", 4 },	{ "new", 5 },
      { "idle", 6 },	 { "downloadList", 7 }, { "download", 8 },
      { "upload", 9 },	 { "fileContent", 10 } };

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
  void sendFileContent ( int clientId );
  void disconnectClient ( int clientId );
  void sendFilesDownload ( int clientId );
  void sendFileContentDownload ( int clientId );
  void uploadFile ( int clientId, bool isNew = false );
  void receiveFileContent ( int clientId );

  // aux
  int getAvailable ( );
  int checkFileFull ( int clientId, std::string filename );
  bool *available;
  void getFiles ( );
  void updateClientsFiles ( int clientId, std::string filename,
                bool disconnected = false );
  std::string *clientsUsernames;
  std::vector< int > nOfUsersFile; // cati clienti editeaza un fisier
  std::vector< std::pair< int, int > >
      clientsUsingFile; // ce clienti editeaza fisierele

public slots:
  void handleResults ( const QString &msg ) { emit logMessage ( msg ); };

signals:
  void logMessage ( const QString &msg );
  // void startListening ( );
  // void stopAllClients ( );
};

#endif // SERVERMAIN_H
