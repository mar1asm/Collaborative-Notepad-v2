#include "clientmain.h"

ClientMain::ClientMain ( QObject *parent ) : QObject ( parent ) {
  hasServerClosed = false;
}

int ClientMain::connectToServer ( char *address, int port ) {
  if ( ( socketDescriptor = socket ( AF_INET, SOCK_STREAM, 0 ) ) == -1 ) {
    perror ( "[client] Eroare la socket().\n" );
    return errno;
  }
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr ( address );
  server.sin_port = htons ( port );
  if ( ::connect ( socketDescriptor, ( struct sockaddr * ) &server,
           sizeof ( struct sockaddr ) ) == -1 ) {
    perror ( "[client]Eroare la connect().\n" );
    return errno;
  }

  sendRequest ( { "username", ( char * ) this->username.data ( ) } );
  spawnListeningThread ( );
  return 0;
}

void ClientMain::spawnListeningThread ( ) {
  listeningThread = std::thread ( &ClientMain::threadCallback, this );
  listeningThread.detach ( );
}

int ClientMain::threadCallback ( ) {
  int error = 0;
  while ( ! error ) {
    error = listen ( );
  }
  return 0;
}

int ClientMain::listen ( ) {
  int length;
  int readValue;
  readValue = read ( socketDescriptor, &length, sizeof ( length ) );
  if ( readValue <= 0 ) {
    if ( ( errno == EINVAL && hasServerClosed ) || readValue == 0 ) {
      hasServerClosed = false;
      return -1;
    } else if ( readValue < 0 ) {
      perror ( "[client]Eroare la read() de la server.\n" );
      return errno;
    }
  }
  char msg[ length + 1 ];
  if ( read ( socketDescriptor, msg, length ) < 0 ) {
    perror ( "[client]Eroare la read() de la server.\n" );
    return errno;
  }
  msg[ length ] = '\0';

  std::cout << "Am primit " << msg << "\n";
  processServerMessage ( msg );
  return 0;
}

void ClientMain::processServerMessage ( char *message ) {
  switch ( messagesNumbers.at ( message ) ) {
  case -1:
    emit serverClosed ( ); // dc nu merge?
    break;
  case 1: // list
    listFiles ( );
    break;
  case 2: // update
    break;
  case 3: // perm
    break;
  case 4: // idle
    break;
  default:
    break;
  }
  return;
}

void ClientMain::listFiles ( ) {
  int numberOfFiles;
  if ( read ( socketDescriptor, &numberOfFiles, sizeof ( numberOfFiles ) ) <
       0 ) {
    perror ( "[client]Eroare la read() de la server.\n" );
    return;
  }
  QVector< QPair< QString, int > > files;
  for ( int i = 0; i < numberOfFiles; i++ ) {
    int filenameLength;
    if ( read ( socketDescriptor, &filenameLength, sizeof ( filenameLength ) ) <
     0 ) {
      perror ( "[client]Eroare la read() de la server.\n" );
      return;
    }

    char file[ filenameLength + 1 ];

    if ( read ( socketDescriptor, file, filenameLength ) < 0 ) {
      perror ( "[client]Eroare la read() de la server.\n" );
      return;
    }

    int clientsConnected;

    if ( read ( socketDescriptor, &clientsConnected,
        sizeof ( clientsConnected ) ) < 0 ) {
      perror ( "[client]Eroare la read() de la server.\n" );
      return;
    }

    // std::cout << file << "\n";
    files.push_back ( qMakePair ( QString ( file ), clientsConnected ) );
  }

  emit openDialog ( files );
  // ListOfFilesWidget *listWidget;
  // listWidget = new ListOfFilesWidget;
  // listWidget->setColumnCount ( 1 );
  // listWidget->setRowCount ( numberOfFiles );
}

void ClientMain::closeConnection ( ) {
  shutdown ( this->socketDescriptor, SHUT_RDWR );
}

void ClientMain::sendRequest ( std::initializer_list< std::string > msgs ) {
  // nu credeam ca o sa ajung sa folosesc asta vreodata
  for ( std::string msg : msgs ) {
    std::cout << "Am trimis " << msg << "\n";
    int requestLength = msg.size ( );
    if ( write ( socketDescriptor, &requestLength, sizeof ( int ) ) <= 0 ) {
      perror ( "[client]Eroare la write() spre server.\n" );
    }
    if ( write ( socketDescriptor, msg.data ( ), requestLength ) <= 0 ) {
      perror ( "[client]Eroare la write() spre server.\n" );
    }
  }
}

void ClientMain::setUsername ( std::string username ) {
  this->username = username;
}
