#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include "servermain.h"
#include <QMainWindow>

namespace Ui {
class ServerWindow;
}

class ServerWindow : public QMainWindow {
  Q_OBJECT

private slots:
  void logMessage ( const QString &msg );

  void on_toggleServer_clicked ( );

public:
  explicit ServerWindow ( QWidget *parent = 0 );
  ~ServerWindow ( );

private:
  Ui::ServerWindow *ui;
  serverMain *servermain;
  bool isRunning = false;
};

#endif // SERVERWINDOW_H
