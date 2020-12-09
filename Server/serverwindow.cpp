#include "serverwindow.h"
#include "ui_serverwindow.h"

ServerWindow::ServerWindow ( QWidget *parent )
    : QMainWindow ( parent ), ui ( new Ui::ServerWindow ),
      filesServer ( new FilesServer ( this ) ) {
  ui->setupUi ( this );
}

ServerWindow::~ServerWindow ( ) { delete ui; }

void ServerWindow::on_toggleServer_toggled ( bool checked ) {
  if ( checked ) {
    ui->toggleServer->setText ( "Stop Server" );
  } else {
    ui->toggleServer->setText ( "Start Server" );
  }
}
