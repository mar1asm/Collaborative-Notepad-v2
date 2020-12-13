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
  safeShutdown = false;
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
    safeShutdown = true;
    // shutdown(Listen_fd, SHUT_RDWR);
    if ( available[ i ] ) {
      sendMessage ( clientSocketDescriptor[ i ], { "serverClosed" } );
      shutdown ( clientSocketDescriptor[ i ], SHUT_RDWR );
      shutdown ( serverSocketDescriptor, SHUT_RDWR );
      close ( serverSocketDescriptor ); //??????????
      // threadPool[ i ].join ( );
    }
    // sa trimit ceva mesaj sa stie
    // clientul ca s a oprit serverul
    // sa opresc si restul threadurilor
  }
  return;
}

void serverMain::spawnSpawningThread ( ) {
  std::thread *t = new std::thread ( &serverMain::spawnThreads, this );
  t->detach ( );
  return;
}

int serverMain::threadCallback ( int clientId ) {
  int error = 0;
  while ( ! error ) {
    error = processRequest ( clientId );
  }
  return 0;
}
void serverMain::spawnThreads ( ) {
  for ( ;; ) {
    int clientId = getAvailable ( ); // functie ce returneaza un Id ce nu este
                     // utilizat de alt client
    if ( ( clientSocketDescriptor[ clientId ] = waitForClients ( ) ) != -1 ) {
      available[ clientId ] = false;
      threadPool[ clientId ] =
      std::thread ( &serverMain::threadCallback, this, clientId );
      threadPool[ clientId ].detach ( );
    }
  }
}

int serverMain::waitForClients ( ) {
  socklen_t length = sizeof ( fromSocket );
  // int tempSocketDescriptor;

  bzero ( &fromSocket, sizeof ( fromSocket ) );
  lock.lock ( );
  int client;
  if ( ( client = accept ( serverSocketDescriptor,
               ( struct sockaddr * ) &fromSocket, &length ) ) <
       0 ) {
    if ( errno == EINVAL && safeShutdown )
      return 0;
    return -1;
  }
  lock.unlock ( );
  return client;
}

void serverMain::sendMessage ( int clientId,
                   std::initializer_list< std::string > msgs,
                   bool sendLength ) {
  // nu credeam ca o sa ajung sa folosesc asta vreodata
  for ( std::string msg : msgs ) {
    std::cout << "Am trimis: " << msg << "\n";
    if ( sendLength ) {
      int messageLength = msg.length ( );
      if ( write ( clientSocketDescriptor[ clientId ], &messageLength,
           sizeof ( int ) ) <= 0 ) {
    perror ( "[client]Eroare la write() spre client.\n" );
      }
      if ( write ( clientSocketDescriptor[ clientId ], msg.data ( ),
           messageLength ) <= 0 ) {
    perror ( "[client]Eroare la write() spre client.\n" );
      }
    } else {
      int temp = std::stoi ( msg );

      if ( write ( clientSocketDescriptor[ clientId ], &temp,
           sizeof ( int ) ) <= 0 ) {
    perror ( "[client]Eroare la write() spre client.\n" );
      }
    }
  }
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
  std::cout << "Am primit: " << msg << "\n";

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
    emit logMessage ( "unknow comm" );
  }
  return 0;
}

void serverMain::setUsername ( int clientId ) {
  int length;
  if ( read ( clientSocketDescriptor[ clientId ], &length, sizeof ( length ) ) <
       0 ) {
    perror ( "eroare la read de la client" );
    emit logMessage ( QStringLiteral ( "eroare la read de la client" ) );
  }

  char msg[ length + 1 ];
  if ( read ( clientSocketDescriptor[ clientId ], msg, length ) < 0 ) {
    perror ( "eroare la read de la client" );
    emit logMessage ( QStringLiteral ( "eroare la read de la client" ) );
  }
  msg[ length ] = '\0';
  clientsUsernames[ clientId ] = msg;

  std::cout << clientsUsernames[ clientId ] << " s-a conectat\n";
  std::string smsg;
  smsg += clientsUsernames[ clientId ];
  smsg += " s-a conectat";
  emit logMessage ( QString ( smsg.data ( ) ) );
}

void serverMain::sendListOfFiles ( int clientId ) {
  const int numberOfFiles = fileNames.size ( );
  this->sendMessage ( clientId, { "list" } );
  this->sendMessage ( clientId, { std::to_string ( numberOfFiles ).data ( ) },
              false );

  for ( std::string file : fileNames ) {
    // QFileInfo fi ( *file );
    // std::cout << fi.fileName ( ).toStdString ( ) << "\n";
    this->sendMessage ( clientId, { file } );
    //}
  }
}

void serverMain::updateFile ( int clientId ) {}

void serverMain::createFile ( int clientId ) {}

void serverMain::disconnectClient ( int clientId ) {}

void serverMain::clientDisconnected (
    int clientId ) { // cum detectez daca clientul a inchis fara sa spuna?
  available[ clientId ] = true;
  std::cout << clientsUsernames[ clientId ] << " s-a deconectat\n";
  std::string smsg;
  smsg += clientsUsernames[ clientId ];
  smsg += " s-a deconectat";
  emit logMessage ( QString ( smsg.data ( ) ) );
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
      files.push_back ( refFile ); //??????????????/
      QFileInfo fileInfo ( file );
      fileNames.push_back ( fileInfo.fileName ( ).toStdString ( ) );
    }
  }
}

int serverMain::getAvailable ( ) {
  for ( int i = 0; i < this->threadsCount; i++ )
    if ( available[ i ] )
      return i;
  return -1;
}
