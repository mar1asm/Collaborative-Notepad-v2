#ifndef CLIENTMAIN_H
#define CLIENTMAIN_H

#include <QObject>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

class ClientMain : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY ( ClientMain )
public:
  explicit ClientMain ( QObject *parent = nullptr );
  int connectToServer ( char *address, int port );

private:
  int port;
  int socketDescriptor;
  struct sockaddr_in server;
  char *address;
  void trimiteMesaj ( );
};

#endif // CLIENTMAIN_H
