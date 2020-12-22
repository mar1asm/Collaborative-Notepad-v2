#ifndef CLIENTMAIN_H
#define CLIENTMAIN_H

#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <QObject>
#include <QTableWidget>
#include <QTextStream>
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
  void sendRequest ( std::initializer_list< std::string > msgs,
             bool hasLength = true );
  void setUsername ( std::string username );
  void closeConnection ( );
  int uploadFile ( QString fileName, QString fileContent );

private:
  const std::unordered_map< std::string, int > messagesNumbers {
      { "serverClosed", -1 }, { "list", 1 },	 { "file", 2 }, { "update", 3 },
      { "downloadList", 4 },  { "download", 5 }, { "idle", 6 } };
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
  void processServerMessage ( std::string message );
  void openFile ( );
  std::string readMessage ( bool hasLength = true );
  void downloadFiles ( );
  void download ( );

public slots:
  void on_refreshFiles ( );
  void on_deleteFile ( int fileId, QString filename );
  void on_OpenFile ( int fileId, QString filename );
signals:
  void serverClosed ( );
  void logMessage ( std::string type, std::string sub, std::string message );
  void openDialog ( QVector< QPair< QString, int > > files );
  void closeDialog ( );
  void openDownloadDialog ( QVector< QString > files );
  void closeDownloadDialog ( );
  void addLine ( QString line );
  void refreshFiles ( QVector< QPair< QString, int > > files );
  void downloadFile ( QString file );
};

#endif // CLIENTMAIN_H
