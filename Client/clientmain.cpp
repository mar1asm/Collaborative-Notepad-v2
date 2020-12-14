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
  std::string error = "ok";
  while ( error != "" ) {
    error = listen ( );
  }
  return 0;
}

int ClientMain::listen ( ) {
  std::string message;
  message = readMessage ( );
  processServerMessage ( message );
  return 0;
}

std::string ClientMain::readMessage ( bool hasLength ) {
  if ( hasLength ) {
    int length;
    int readError = read ( socketDescriptor, &length, sizeof ( length ) );
    if ( readError <= 0 ) {
      if ( ( errno == EINVAL ) || readError == 0 ) {
    hasServerClosed = false;
    return "";
      } else if ( readError < 0 ) {
    perror ( "[client]Eroare la read() de la server.\n" );
    return "";
      }
    }

    char msg[ length + 1 ];
    if ( read ( socketDescriptor, msg, length ) < 0 ) {
      perror ( "[client]Eroare la read() de la server.\n" );
      return "";
    }
    msg[ length ] = '\0';
    return msg;
  } else {
    int msg;
    int readError = read ( socketDescriptor, &msg, sizeof ( msg ) );
    if ( readError <= 0 ) {
      if ( ( errno == EINVAL ) || readError == 0 ) {
    hasServerClosed = false;
    return "";
      } else if ( readError < 0 ) {
    perror ( "[client]Eroare la read() de la server.\n" );
    return "";
      }
    }
    return std::to_string ( msg );
  }
}

void ClientMain::processServerMessage ( std::string message ) {

  switch ( messagesNumbers.at ( message ) ) {
  case -1:
    emit serverClosed ( ); // dc nu merge?
    break;
  case 1: // list
    listFiles ( );
    break;
  case 2: // file content
    openFile ( );
    break;
  case 3: // update
    break;
  case 4: // perm
    break;
  case 5: // idle
    break;
  default:
    break;
  }
  return;
}

void ClientMain::openFile ( ) {
  std::string temp = readMessage ( false );
  if ( temp == "" )
    return;
  int numberOfFiles = stoi ( temp );
  for ( int i = 0; i < numberOfFiles; i++ ) {
    std::string line = readMessage ( );

    std::cout << "line e: " << line << "\n";
    if ( line == "" )
      return;
    emit addLine ( QString ( line.data ( ) ) );
  }
}

void ClientMain::listFiles ( ) {
  int numberOfFiles;

  numberOfFiles = std::stoi ( readMessage ( false ) );
  QVector< QPair< QString, int > > files;
  for ( int i = 0; i < numberOfFiles; i++ ) {

    std::string filename;

    filename = readMessage ( );

    int clientsConnected;

    clientsConnected = std::stoi ( readMessage ( false ) );
    files.push_back (
    qMakePair ( QString ( filename.data ( ) ), clientsConnected ) );
  }

  emit openDialog ( files );
}

void ClientMain::closeConnection ( ) {
  shutdown ( this->socketDescriptor, SHUT_RDWR );
}

void ClientMain::sendRequest ( std::initializer_list< std::string > msgs,
                   bool hasLength ) {
  // nu credeam ca o sa ajung sa folosesc asta vreodata
  for ( std::string msg : msgs ) {
    if ( hasLength ) {
      // std::cout << "Am trimis " << msg << "\n";
      int requestLength = msg.size ( );
      std::cout << "mesajul e " << msg << " si lungimea e " << requestLength
        << "\n";
      if ( write ( socketDescriptor, &requestLength, sizeof ( int ) ) <= 0 ) {
    perror ( "[client]Eroare la write() spre server.\n" );
      }
      if ( write ( socketDescriptor, msg.data ( ), requestLength ) <= 0 ) {
    perror ( "[client]Eroare la write() spre server.\n" );
      }
    } else {
      int temp = std::stoi ( msg );
      std::cout << " am mai trimis si altceva? ";
      if ( write ( socketDescriptor, &temp, sizeof ( int ) ) <= 0 ) {
    perror ( "[client]Eroare la write() spre server.\n" );
      }
    }
  }
}

void ClientMain::setUsername ( std::string username ) {
  this->username = username;
}

void ClientMain::on_refreshFiles ( ) {}

void ClientMain::on_OpenFile ( int fileId, QString filename ) {
  std::cout << "s-a apelat on_openFile\n";
  std::string editedFilename;
  editedFilename = filename.toStdString ( );
  editedFilename =
      editedFilename.substr ( 0, editedFilename.find_last_of ( ' ' ) );
  sendRequest ( { "file", editedFilename } );
  emit closeDialog ( );
}

void ClientMain::on_deleteFile ( int fileId, QString filename ) {
  std::cout << fileId << " " << filename.toStdString ( ) << "\n";
  emit closeDialog ( );
}
