#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include "filesserver.h"
#include <QMainWindow>

namespace Ui {
class ServerWindow;
}

class ServerWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit ServerWindow ( QWidget *parent = 0 );
  ~ServerWindow ( );

private slots:
  void logMessage ( const QString &msg );
  void on_toggleServer_clicked ( );

private:
  Ui::ServerWindow *ui;
  FilesServer *filesServer;
  bool isRunning = false;
};

#endif // SERVERWINDOW_H
