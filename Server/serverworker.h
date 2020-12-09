#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include <QObject>
#include <QReadWriteLock>
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

class ServerWorker : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY ( ServerWorker )

public:
  explicit ServerWorker ( QObject *parent = nullptr );
  QString userName ( ) const;
  // void setUserName ( const QString &userName );
  // void sendData ( const QJsonObject &json );
public slots:
  // void disconnectFromClient ( );
private slots:
  void receiveData ( );
signals:
  // void dataReceived ( const QJsonObject &jsonDoc );
  // void disconnectedFromClient ( );
  // void error ( );
  void logMessage ( const QString &msg );

private:
  int socketDescriptor;
  QString c_userName;
  int client;
  mutable QReadWriteLock m_userNameLock;
  struct sockaddr_in fromSocket;
};

#endif // SERVERWORKER_H
