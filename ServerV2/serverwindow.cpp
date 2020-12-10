#include "serverwindow.h"
#include "ui_serverwindow.h"
#include <QMessageBox>

ServerWindow::ServerWindow ( QWidget *parent )
    : QMainWindow ( parent ), ui ( new Ui::ServerWindow ),
      servermain ( new serverMain ( this ) ) {
  ui->setupUi ( this );
  connect ( servermain, &serverMain::logMessage, this,
        &ServerWindow::logMessage );
}

ServerWindow::~ServerWindow ( ) { delete ui; }

void ServerWindow::on_toggleServer_clicked ( ) {
  if ( isRunning ) {
    isRunning = false;
    servermain->stopServer ( );
    ui->toggleServer->setText ( tr ( "Start Server" ) );
    logMessage ( QStringLiteral ( "Server Stopped" ) );
  } else {
    if ( servermain->startServer ( ) ) {
      QMessageBox::critical ( this, tr ( "Error" ),
                  tr ( "Unable to start the server" ) );
      return;
    }
    isRunning = true;
    ui->toggleServer->setText ( tr ( "Stop Server" ) );
    servermain->spawnThread ( );
  }
}

void ServerWindow::logMessage ( const QString &msg ) {
  ui->serverLog->appendPlainText ( msg + QLatin1Char ( '\n' ) );
}
