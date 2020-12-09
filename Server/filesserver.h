#ifndef FILESSERVER_H
#define FILESSERVER_H
#include <QObject>
#include <QVector>
class QThread;
class ServerWorker;

class FilesServer : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY ( FilesServer )
public:
  explicit FilesServer ( QObject *parent = nullptr );

protected:
  void startServer ( );
  void stopServer ( );
  int incomingConnection ( int socketDescriptor );

private:
  ~FilesServer ( );
  const int idealThreadCount;
  QVector< QThread * > availableThreads;
  QVector< int > threadsLoad;
  QVector< ServerWorker * > clients;
signals:
  void logMessage ( const QString &msg );
  void stopAllClients ( );
};

#endif // FILESSERVER_H
