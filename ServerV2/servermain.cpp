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
    if ( available[ i ] ) { // aici ceva sa trimita cum trebuie. prob problema e
                // ca intai ii face available [clientId]= false
                // atunci cand da fail readul. dar prob da fail
                // readul pentru ca e inchid deja socketu
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

int serverMain::processRequest ( int clientId ) {

  std::string msg;
  msg = readMessage ( clientSocketDescriptor[ clientId ] );
  if ( msg.size ( ) == 0 ) {
    return -1;
  }
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
    sendFileContent ( clientId );
    break;
  case 4:
    updateFile ( clientId );
    break;
  case 5: // new
    uploadFile ( clientId, true );
    break;
  case 6:
    disconnectClient ( clientId );
    break;
  case 7:
    sendFilesDownload ( clientId );
    break;
  case 8:
    sendFileContentDownload ( clientId );
    break;
  case 9: // upload
    uploadFile ( clientId );
    break;
  case 10: // upload
    receiveFileContent ( clientId );
    break;

  default:
    std::cout << "unknown comm";
    emit logMessage ( "unknow comm" );
  }
  return 0;
}

void serverMain::uploadFile ( int clientId, bool isNew ) {

  std::string filename = readMessage ( clientSocketDescriptor[ clientId ] );
  std::string message;

  std::string action = isNew ? "creeze" : "uploadeze";

  message = clientsUsernames[ clientId ] + " a solicitat sa " + action +
        " fisierul " + filename;
  emit logMessage ( QString ( message.data ( ) ) );

  for ( std::string file : fileNames ) {
    if ( file == filename ) {
      std::cout << file << " " << filename << "\n";
      message = "Fisierul cu numele" + filename +
        "exista deja. Am trimis notificare catre " +
        clientsUsernames[ clientId ];
      emit logMessage ( QString ( message.data ( ) ) );
      sendMessage ( clientSocketDescriptor[ clientId ], { "used" } );
      return;
    }
  }
  message = "Am creat fisierul cu numele " + filename +
        ". Am trimis notificare catre " + clientsUsernames[ clientId ];
  emit logMessage ( QString ( message.data ( ) ) );

  sendMessage ( clientSocketDescriptor[ clientId ], { "ok" } );
  fileNames.push_back ( filename );
  clientsUsingFile.push_back ( std::make_pair ( clientId, -1 ) );
  nOfUsersFile.push_back ( 1 );
  if ( isNew ) {
    QFile file ( "documents/" + QString ( filename.data ( ) ) );
    file.open ( QIODevice::ReadWrite );
    file.close ( );
  }
}

void serverMain::receiveFileContent ( int clientId ) {

  std::string filename = readMessage ( clientSocketDescriptor[ clientId ] );
  std::string lines = readMessage ( clientSocketDescriptor[ clientId ], false );
  std::cout << lines << "\n";
  int numberOfLines = stoi ( lines );

  std::string message = clientsUsernames[ clientId ] +
            " a trimis continutul fisierului " + filename;
  emit logMessage ( QString ( message.data ( ) ) );
  QFile file ( "documents/" + QString ( filename.data ( ) ) );

  file.open ( QIODevice::WriteOnly | QIODevice::Append );
  for ( int i = 0; i < numberOfLines; i++ ) {
    QString line = readMessage ( clientSocketDescriptor[ clientId ] ).data ( );
    line += '\n';
    file.write ( line.toStdString ( ).data ( ) );
  }
}

void serverMain::setUsername ( int clientId ) {
  int length;
  std::string msg = readMessage ( clientSocketDescriptor[ clientId ] );
  if ( msg.size ( ) == 0 ) {
    emit logMessage ( QStringLiteral ( "eroare la read de la client" ) );
    perror ( "eroare la read de la client" );
    return;
  }

  if ( msg == "quit" ) {
    clientDisconnected ( clientId );
    return;
  }

  clientsUsernames[ clientId ] = msg;

  std::cout << clientsUsernames[ clientId ] << " s-a conectat\n";
  msg += " s-a conectat!";
  emit logMessage ( QString ( msg.data ( ) ) );
}

void serverMain::sendListOfFiles ( int clientId ) {
  const int numberOfFiles = fileNames.size ( );
  std::string message;
  message = clientsUsernames[ clientId ] +
        " a solicitat lista de fisiere disponibile pentru deschidere.";
  emit logMessage ( QString ( message.data ( ) ) );
  message = "Am trimis lista fisierelor disponibile pentru deschidere catre " +
        clientsUsernames[ clientId ];
  emit logMessage ( QString ( message.data ( ) ) );

  sendMessage ( clientSocketDescriptor[ clientId ], { "list" } );
  sendMessage ( clientSocketDescriptor[ clientId ],
        { std::to_string ( numberOfFiles ).data ( ) }, false );
  int fileIndex = 0;
  for ( std::string file : fileNames ) {

    // QFileInfo fi ( *file );
    // std::cout << fi.fileName ( ).toStdString ( ) << "\n";
    sendMessage ( clientSocketDescriptor[ clientId ], { file } );
    sendMessage ( clientSocketDescriptor[ clientId ],
          { std::to_string ( nOfUsersFile[ fileIndex ] ) }, false );
    fileIndex++;
    //}
  }
}

void serverMain::sendFilesDownload ( int clientId ) {
  const int numberOfFiles = fileNames.size ( );
  std::string message;
  message = clientsUsernames[ clientId ] +
        " a solicitat lista de fisiere disponibile pentru descarcare.";
  emit logMessage ( QString ( message.data ( ) ) );
  message = "Am trimis lista fisierelor disponibile pentru descarcare catre " +
        clientsUsernames[ clientId ];
  emit logMessage ( QString ( message.data ( ) ) );
  sendMessage ( clientSocketDescriptor[ clientId ], { "downloadList" } );
  sendMessage ( clientSocketDescriptor[ clientId ],
        { std::to_string ( numberOfFiles ).data ( ) }, false );
  int fileIndex = 0;
  for ( std::string file : fileNames ) {

    // QFileInfo fi ( *file );
    // std::cout << fi.fileName ( ).toStdString ( ) << "\n";
    sendMessage ( clientSocketDescriptor[ clientId ], { file } );
    fileIndex++;
    //}
  }
}

int serverMain::checkFileFull ( int clientId, std::string filename ) {
  for ( unsigned long i = 0; i < fileNames.size ( ); i++ ) {
    if ( fileNames[ i ] == filename ) {
      if ( nOfUsersFile[ i ] == 2 ) {
    sendMessage ( clientSocketDescriptor[ clientId ], { "max" } );
    return -1;
      } else {
    sendMessage ( clientSocketDescriptor[ clientId ], { "notMax" } );
    return 0;
      }
    }
  }
  return -2;
}

void serverMain::updateClientsFiles ( int clientId, std::string filename,
                      bool disconnected ) {
  for ( unsigned long i = 0; i < clientsUsingFile.size ( ); i++ ) {
    if ( ! disconnected ) {
      if ( fileNames[ i ] == filename ) {
    nOfUsersFile[ i ]++;
    if ( clientsUsingFile[ i ].first == -1 )
      clientsUsingFile[ i ].first = clientId;
    else
      clientsUsingFile[ i ].second = clientId;
    continue;
      }
    }
    std::pair< int, int > clients;
    clients = clientsUsingFile[ i ];
    if ( clients.first == clientId ) {
      clients.first = -1;
      nOfUsersFile[ i ]--;
    }
    if ( clients.second == clientId ) {
      clients.second = -1;
      nOfUsersFile[ i ]--;
    }
    clientsUsingFile[ i ] = clients;
  }
}

void serverMain::sendFileContent ( int clientId ) {
  std::string msg = readMessage ( clientSocketDescriptor[ clientId ] );
  std::string message;
  QString filename = QString ( msg.data ( ) );
  sendMessage ( clientSocketDescriptor[ clientId ], { "file" } );

  int fileFull = checkFileFull ( clientId, filename.toStdString ( ) );
  if ( fileFull ) {
    std::string action = readMessage ( clientSocketDescriptor[ clientId ] );
    if ( action == "cancel" )
      return;
  }

  message =
      clientsUsernames[ clientId ] + " a solicitat sa deschida fisierul " + msg;
  emit logMessage ( QString ( message.data ( ) ) );

  // ceva max length de trimis
  QFile file ( "documents/" + filename );
  if ( ! file.open ( QIODevice::ReadOnly ) ) {
    sendMessage ( clientSocketDescriptor[ clientId ], { "error" } );
    return;
  } else {

    int lineCount = 0;
    QTextStream in ( &file );
    while ( ! in.atEnd ( ) ) {
      in.readLine ( );
      lineCount++;
    }
    in.seek ( 0 );
    sendMessage ( clientSocketDescriptor[ clientId ],
          { std::to_string ( lineCount ) }, false );
    while ( ! in.atEnd ( ) ) {
      QString line ( in.readLine ( ) );
      sendMessage ( clientSocketDescriptor[ clientId ],
            { line.toStdString ( ) } );
    }
  }
  updateClientsFiles ( clientId, filename.toStdString ( ) );
}

void serverMain::updateFile ( int clientId ) {}

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
  int fileIndex = 0;
  while ( iterator.hasNext ( ) ) {
    QFile *refFile;
    QFile file ( iterator.next ( ) ); // citesc fisierele
    if ( file.open ( QIODevice::ReadOnly ) ) {
      refFile = &file;
      files.push_back ( refFile ); //??????????????/
      QFileInfo fileInfo ( file );
      fileNames.push_back ( fileInfo.fileName ( ).toStdString ( ) );
      nOfUsersFile.push_back ( 0 );
      clientsUsingFile.push_back ( std::make_pair ( -1, -1 ) );
    }
    fileIndex++;
  }
}

int serverMain::getAvailable ( ) {
  for ( int i = 0; i < this->threadsCount; i++ )
    if ( available[ i ] )
      return i;
  return -1;
}

void serverMain::sendFileContentDownload ( int clientId ) {
  QString filename (
      readMessage ( clientSocketDescriptor[ clientId ] ).data ( ) );
  QFile file ( "documents/" + filename );
  if ( ! file.open ( QIODevice::ReadOnly ) ) {
    sendMessage ( clientSocketDescriptor[ clientId ], { "error" } );
    return;
  } else {
    sendMessage ( clientSocketDescriptor[ clientId ], { "download" } );
  }

  std::string message;
  message = clientsUsernames[ clientId ] +
        " a solicitat sa descarce fisierul " + filename.toStdString ( );
  emit logMessage ( QString ( message.data ( ) ) );
  message = "Am trimis fisierul " + clientsUsernames[ clientId ] + " catre " +
        clientsUsernames[ clientId ];
  emit logMessage ( QString ( message.data ( ) ) );

  int lineCount = 0;
  QTextStream in ( &file );
  while ( ! in.atEnd ( ) ) {
    in.readLine ( );
    lineCount++;
  }
  in.seek ( 0 );
  sendMessage ( clientSocketDescriptor[ clientId ],
        { std::to_string ( lineCount ) }, false );
  while ( ! in.atEnd ( ) ) {
    QString line ( in.readLine ( ) );
    sendMessage ( clientSocketDescriptor[ clientId ],
          { line.toStdString ( ) } );
  }
}
