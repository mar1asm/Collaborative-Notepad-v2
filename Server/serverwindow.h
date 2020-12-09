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
  void on_toggleServer_toggled ( bool checked );
  void logMessage ( const QString &msg );

private:
  Ui::ServerWindow *ui;
  FilesServer *filesServer;
};

#endif // SERVERWINDOW_H
