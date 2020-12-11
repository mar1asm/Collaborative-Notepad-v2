#include "servermain.h"
#include <QTimer>
#include <functional>
#include <thread>

serverMain::serverMain ( QObject *parent ) {
  this->threadsCount = std::thread::hardware_concurrency ( );
  this->clientSocketDescriptor = new int[ threadsCount ];
  this->available = new bool[ threadsCount ]; // nu merge asa{ true };
  this->threadPool = new std::thread[ threadsCount ];
  this->clientsUsernames = new std::string[ threadsCount ];
  for ( int i = 0; i < this->threadsCount; i++ ) {
    this->available[ i ] = true;
  }
  this->getFiles ( );
}

serverMain::~serverMain ( ) {
  // sa vad sa nu ramana ceva deschis
}

int serverMain::startServer ( ) {
  if ( ( serverSocketDescriptor = socket ( AF_INET, SOCK_STREAM, 0 ) ) == -1 ) {
    emit logMessage ( QStringLiteral ( "Eroare la socket())" ) );
    perror ( "[server]Eroare la socket().\n" );
    return errno;
  }
  int on = 1;
  setsockopt ( serverSocketDescriptor, SOL_SOCKET, SO_REUSEADDR, &on,
           sizeof ( on ) );
  bzero ( &serverSocket, sizeof ( serverSocket ) );
  serverSocket.sin_family = AF_INET;
  serverSocket.sin_addr.s_addr = htonl ( INADDR_ANY );
  serverSocket.sin_port = htons ( PORT );
  if ( bind ( serverSocketDescriptor, ( struct sockaddr * ) &serverSocket,
          sizeof ( struct sockaddr ) ) == -1 ) {
    emit logMessage ( QStringLiteral ( "Eroare la bind()" ) );
    perror ( "[server]Eroare la bind().\n" );
    return errno;
  }

  if ( listen ( serverSocketDescriptor, 2 ) == -1 ) {
    emit logMessage ( QStringLiteral ( "Eroare la listen()" ) );
    perror ( "[server]Eroare la listen().\n" );
    return errno;
  }

  emit logMessage ( QStringLiteral ( "Serverul a pornit" ) );
  fprintf ( stdout, "Serverul a pornit \n" );
  return 0;
}

void serverMain::stopServer ( ) {
  for ( int i = 0; i < threadsCount; i++ ) {
    if ( available[ i ] ) {
      close ( clientSocketDescriptor[ i ] );
      threadPool[ i ].join ( );
    }
    // sa trimit ceva mesaj sa stie
    // clientul ca s a oprit serverul
    // sa opresc si restul threadurilor
  }
  return;
}

void serverMain::spawnThread ( ) {
  // printf ( "am ajuns aici\n" );
  for ( ;; ) {
    int clientId = getAvailable ( );
    printf ( "clientul e %d\n", clientId );
    if ( ( clientSocketDescriptor[ clientId ] = waitForClients ( ) ) != -1 ) {
      // printf ( "aici n ar trebui sa afiseze\n" );
      available[ clientId ] = false;
      threadPool[ clientId ] =
      std::thread ( &serverMain::threadCallback, this, clientId );
      threadPool[ clientId ].detach ( );
    }
  }
}

int serverMain::threadCallback ( int clientId ) {
  int error = 0;
  while ( ! error ) {
    // printf ( "surprinzator a ajuns aici\n" );
    error = processRequest ( clientId );
  }
  return 0;
}

int serverMain::waitForClients ( ) {
  socklen_t length = sizeof ( fromSocket );
  // int tempSocketDescriptor;

  bzero ( &fromSocket, sizeof ( fromSocket ) );
  int client;
  // int client = malloc ( sizeof ( int ) );
  if ( ( client = accept ( serverSocketDescriptor,
               ( struct sockaddr * ) &fromSocket, &length ) ) <
       0 ) {
    emit logMessage ( QStringLiteral (
    "nu merge acceptul dar oare de ce nu merge daca l am setat bn " ) );
    perror ( "[thread]Eroare la accept().\n" );
    return -1;
  }
  // printf ( "s-a conectat clientul %d\n", client );
  emit logMessage (
      QStringLiteral ( "s-a conectat clientul %1\n" ).arg ( client ) );
  return client;
}

int serverMain::processRequest ( int clientId ) {
  int length;
  if ( read ( clientSocketDescriptor[ clientId ], &length, sizeof ( length ) ) <
       0 ) {
    perror ( "eroare la read de la client" );
    return -1;
  }

  char msg[ length + 1 ];
  if ( read ( clientSocketDescriptor[ clientId ], msg, length ) < 0 ) {
    perror ( "eroare la read de la client" );
    return -1;
  }

  msg[ length ] = '\0';

  // ca sa nu am 1000 de if-else-uri

  switch ( requestsNumbers.at ( msg ) ) {
  case 1:
    setUsername ( clientId );
    break;
  case -1:
    clientDisconnected ( clientId );
    return -1;
  case 2:
    sendListOfFiles ( clientId );
    break;
  case 3:
    updateFile ( clientId );
    break;
  case 4:
    createFile ( clientId );
    break;
  case 5:
    disconnectClient ( clientId );
    break;

  default:
    std::cout << "unknown comm";
  }
  return 0;
}

void serverMain::setUsername ( int clientId ) {
  int length;
  if ( read ( clientSocketDescriptor[ clientId ], &length, sizeof ( length ) ) <
       0 ) {
    perror ( "eroare la read de la client" );
  }

  char msg[ length + 1 ];
  if ( read ( clientSocketDescriptor[ clientId ], msg, length ) < 0 ) {
    perror ( "eroare la read de la client" );
  }
  msg[ length ] = '\0';
  clientsUsernames[ clientId ] = msg;

  std::cout << clientsUsernames[ clientId ] << " s-a conectat\n";
}

void serverMain::sendListOfFiles ( int clientId ) {}

void serverMain::updateFile ( int clientId ) {}

void serverMain::createFile ( int clientId ) {}

void serverMain::disconnectClient ( int clientId ) {}

void serverMain::clientDisconnected (
    int clientId ) { // cum detectez daca clientul a inchis fara sa spuna?
  available[ clientId ] = true;
  std::cout << clientsUsernames[ clientId ] << " s-a deconectat\n";
  clientsUsernames[ clientId ] = "";
  close ( clientSocketDescriptor[ clientId ] );
}

void serverMain::getFiles ( ) {
  // printf ( "am ajuns aici \n" );
  if ( ! QDir ( "documents" ).exists ( ) ) {
    // printf ( "nu exista \n" );
    QDir ( ).mkdir ( "documents" );
    return;
  }

  QDirIterator iterator ( "documents", QDirIterator::Subdirectories );
  while ( iterator.hasNext ( ) ) {
    QFile *refFile;
    QFile file ( iterator.next ( ) ); // citesc fisierele
    if ( file.open ( QIODevice::ReadOnly ) ) {
      refFile = &file;
      files.push_back ( refFile );
    }
  }
}

int serverMain::getAvailable ( ) {
  for ( int i = 0; i < this->threadsCount; i++ )
    if ( available[ i ] )
      return i;
  return -1;
}
