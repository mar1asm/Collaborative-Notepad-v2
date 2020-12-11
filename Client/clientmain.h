#ifndef CLIENTMAIN_H
#define CLIENTMAIN_H

#include <QDir>
#include <QInputDialog>
#include <QObject>
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

class ClientMain : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY ( ClientMain )
public:
  explicit ClientMain ( QObject *parent = nullptr );
  int connectToServer ( char *address, int port );
  void sendRequest ( std::initializer_list< char * > msgs );
  void setUsername ( std::string username );

private:
  int port;
  std::thread *listeningThread;
  int socketDescriptor;
  std::string username;
  struct sockaddr_in server;
  char *address;
  void spawnListeningThread ( );
  int listen ( );
  int threadCallback ( );
};

#endif // CLIENTMAIN_H
