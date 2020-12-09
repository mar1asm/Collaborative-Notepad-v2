#include "serverwindow.h"
#include "ui_serverwindow.h"
#include <QMessageBox>

ServerWindow::ServerWindow ( QWidget *parent )
    : QMainWindow ( parent ), ui ( new Ui::ServerWindow ),
      filesServer ( new FilesServer ( this ) ) {
  ui->setupUi ( this );
}

ServerWindow::~ServerWindow ( ) { delete ui; }

void ServerWindow::on_toggleServer_clicked ( ) {
  if ( isRunning ) {
    isRunning = false;
    filesServer->stopServer ( );
    ui->toggleServer->setText ( tr ( "Start Server" ) );
    logMessage ( QStringLiteral ( "Server Stopped" ) );
  } else {
    if ( filesServer->startServer ( ) ) {
      QMessageBox::critical ( this, tr ( "Error" ),
                              tr ( "Unable to start the server" ) );
      return;
    }
    isRunning = true;
    logMessage ( QStringLiteral ( "Server Started" ) );
    ui->toggleServer->setText ( tr ( "Stop Server" ) );
  }
}

void ServerWindow::logMessage ( const QString &msg ) {
  ui->serverLog->appendPlainText ( msg + QLatin1Char ( '\n' ) );
}
