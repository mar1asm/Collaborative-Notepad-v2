#include "serverworker.h"
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

ServerWorker::ServerWorker ( QObject *parent ) : QObject ( parent ) {
  if ( ( socketDescriptor = socket ( AF_INET, SOCK_STREAM, 0 ) ) == -1 )
    throw "[server]Eroare la socket().\n";
  int on = 1;
  setsockopt ( socketDescriptor, SOL_SOCKET, SO_REUSEADDR, &on, sizeof ( on ) );
  bzero ( &server, sizeof ( server ) );
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl ( INADDR_ANY );
  server.sin_port = htons ( PORT );
}

ServerWorker::nuStiuIncaCumSeCheama ( ) {}
