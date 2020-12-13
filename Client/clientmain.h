#ifndef CLIENTMAIN_H
#define CLIENTMAIN_H

#include <QDir>
#include <QInputDialog>
#include <QObject>
#include <QTableWidget>
#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>

class ClientMain : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY ( ClientMain )
public:
  ClientMain ( QObject *parent = nullptr );
  int connectToServer ( char *address, int port );
  void sendRequest ( std::initializer_list< std::string > msgs );
  void setUsername ( std::string username );
  void closeConnection ( );

private:
  const std::unordered_map< std::string, int > messagesNumbers {
      { "serverClosed", -1 },
      { "list", 1 },
      { "update", 2 },
      { "perm", 3 },
      { "idle", 4 } };
  std::atomic< bool > hasServerClosed;
  int port;
  std::thread listeningThread;
  int socketDescriptor;
  std::string username;
  struct sockaddr_in server;
  char *address;
  void spawnListeningThread ( );
  int listen ( );
  int threadCallback ( );
  void listFiles ( );
  void processServerMessage ( char *message );

public slots:
  void on_refreshFiles ( );
  void on_deleteFile ( int fileId, QString filename );
  void on_OpenFile ( int fileId, QString filename );
signals:
  void serverClosed ( );
  void logMessage ( std::string type, std::string sub, std::string message );
  void openDialog ( QVector< QPair< QString, int > > files );
  void closeDialog ( );
};

#endif // CLIENTMAIN_H
