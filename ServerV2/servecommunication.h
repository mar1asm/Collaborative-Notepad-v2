#ifndef SERVECOMMUNICATION_H
#define SERVECOMMUNICATION_H
#include <iostream>
#include <string>
#include <unistd.h>

inline void sendMessage ( int clientSocketDescriptor,
              std::initializer_list< std::string > msgs,
              bool sendLength = true ) {
  // nu credeam ca o sa ajung sa folosesc asta vreodata
  for ( std::string msg : msgs ) {
    if ( msg.size ( ) == 0 )
      msg += "\n";
    std::cout << "Am trimis: " << msg << "\n";
    if ( sendLength ) {
      int messageLength = msg.length ( );

      if ( write ( clientSocketDescriptor, &messageLength, sizeof ( int ) ) <=
       0 ) {
    perror ( "[client]Eroare la write() spre client.\n" );
      }
      if ( write ( clientSocketDescriptor, msg.data ( ), messageLength ) <=
       0 ) {
    perror ( "[client]Eroare la write() spre client.\n" );
      }
    } else {
      int temp = std::stoi ( msg );

      if ( write ( clientSocketDescriptor, &temp, sizeof ( int ) ) <= 0 ) {
    perror ( "[client]Eroare la write() spre client.\n" );
      }
    }
  }
}

inline std::string readMessage ( int clientSocketDescriptor,
                 bool hasLength = true ) {
  if ( hasLength ) {
    int length;
    int readError = read ( clientSocketDescriptor, &length, sizeof ( length ) );
    if ( readError < 0 ) {
      perror ( "[Server] Erroare la read de la client" );
      return "";
    }
    if ( readError == 0 ) {
      return "quit";
    }
    char msg[ length + 1 ];
    readError = read ( clientSocketDescriptor, msg, length );
    if ( readError < 0 ) {
      perror ( "[Server] Erroare la read de la client" );
      return "";
    }
    if ( readError == 0 ) {
      return "quit";
    }
    msg[ length ] = '\0';
    std::cout << "mesajul e " << msg << " si lungimea e " << length << "\n";
    return msg;
  }
  int msg;
  int readError = read ( clientSocketDescriptor, &msg, sizeof ( msg ) );
  if ( readError < 0 ) {
    perror ( "[Server] Erroare la read de la client" );
    return "";
  }
  if ( readError == 0 ) {
    return "quit";
  }
  std::cout << "mesajul e " << msg << "\n";
  return std::to_string ( msg );
}

#endif // SERVECOMMUNICATION_H
