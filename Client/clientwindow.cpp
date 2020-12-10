#include "clientwindow.h"
#include "ui_clientwindow.h"

ClientWindow::ClientWindow ( QWidget *parent )
    : QMainWindow ( parent ), ui ( new Ui::ClientWindow ) {
  ui->setupUi ( this );
  this->setCentralWidget ( ui->textEdit );
  setWindowTitle ( "New file" );
  clientMain = new ClientMain ( this );
}

int ClientWindow::setConnectionData ( int argc, char *argv[] ) {
  if ( argc != 3 ) {
    printf ( "[client] Sintaxa: %s <adresa_server> <port>\n", argv[ 0 ] );
    return -1;
  } // to do sa pun sa citeasca dintr un fisier config
  int port = atoi ( argv[ 2 ] );
  this->port = port;
  this->address = argv[ 1 ];
  return 0;
}

ClientWindow::~ClientWindow ( ) { delete ui; }

bool ClientWindow::helper_isSaved ( ) {
  QString currentText = ui->textEdit->toPlainText ( );
  if ( currentFile == "" ) {
    if ( currentText == "" )
      return true;
    else
      return false;
  }
  QFile savedFile ( currentFile );
  if ( ! savedFile.open ( QFile::ReadOnly | QFile ::Text ) ) {
    QMessageBox::warning ( this, "Warning",
               "Cannot open file : " + savedFile.errorString ( ) );
    return false;
  }
  QTextStream in ( &savedFile );
  QString savedText = in.readAll ( );
  if ( savedText != currentText )
    return false;
  return true;
}

void ClientWindow::on_actionNew_triggered ( ) {
  bool isSaved = helper_isSaved ( );
  if ( ! isSaved ) {
    QMessageBox saveBeforeClosing ( this );
    saveBeforeClosing.setText ( "The file has been modified" );
    saveBeforeClosing.setInformativeText (
    "Do you want to save your changes?" );
    saveBeforeClosing.setStandardButtons (
    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel );
    saveBeforeClosing.setDefaultButton ( QMessageBox::Save );
    int answer = saveBeforeClosing.exec ( );
    switch ( answer ) {
    case QMessageBox::Save:
      on_actionSave_triggered ( );
      break;
    case QMessageBox::Cancel:
      return;
    default:
      break;
    }
  }
  currentFile.clear ( );
  ui->textEdit->setText ( QString ( ) );
  setWindowTitle ( "New file" );
}

void ClientWindow::on_actionFrom_server_triggered ( ) {}

void ClientWindow::on_actionFrom_PC_triggered ( ) {
  QString fileName = QFileDialog::getOpenFileName ( this, "Open file" );
  if ( fileName == nullptr )
    return;
  QFile file ( fileName );
  currentFile = fileName;
  if ( ! file.open ( QIODevice::ReadOnly | QFile ::Text ) ) {
    QMessageBox::warning ( this, "Warning",
               "Cannot open file : " + file.errorString ( ) );
    return;
  }
  setWindowTitle ( fileName );
  QTextStream in ( &file );
  QString text = in.readAll ( );
  ui->textEdit->setText ( text );
  file.close ( );
}

void ClientWindow::on_actionSave_triggered ( ) {
  if ( currentFile == "" ) {
    on_actionSave_as_triggered ( );
    return;
  }
  QFile file ( currentFile );
  if ( ! file.open ( QFile::WriteOnly | QFile ::Text ) ) {
    QMessageBox::warning ( this, "Warning",
               "Cannot save file : " + file.errorString ( ) );
    return;
  }
  QTextStream out ( &file );
  QString text = ui->textEdit->toPlainText ( );
  out << text;
  file.close ( );
}

void ClientWindow::on_actionSave_as_triggered ( ) {
  QString fileName = QFileDialog::getSaveFileName ( this, "Save as" );
  if ( fileName == nullptr )
    return;
  QFile file ( fileName );
  if ( ! file.open ( QFile::WriteOnly | QFile ::Text ) ) {
    QMessageBox::warning ( this, "Warning",
               "Cannot save file : " + file.errorString ( ) );
    return;
  }
  currentFile = fileName;
  setWindowTitle ( fileName );
  QTextStream out ( &file );
  QString text = ui->textEdit->toPlainText ( );
  out << text;
  file.close ( );
}

void ClientWindow::on_actionExit_triggered ( ) { QApplication::quit ( ); }

void ClientWindow::on_actionCopy_triggered ( ) { ui->textEdit->copy ( ); }
void ClientWindow::on_actionPaste_triggered ( ) { ui->textEdit->paste ( ); }
void ClientWindow::on_actionCut_triggered ( ) { ui->textEdit->cut ( ); }
void ClientWindow::on_actionUndo_triggered ( ) { ui->textEdit->undo ( ); }
void ClientWindow::on_actionRedo_triggered ( ) { ui->textEdit->redo ( ); }

void ClientWindow::on_actionToggleConnection_triggered ( ) {
  if ( connected ) {
    this->clientMain->sendRequest ( "quit" );
  } else {
    if ( username == "" ) {
      bool ok;
      QString text = QInputDialog::getText (
      this, tr ( "Set username" ), tr ( "Username:" ), QLineEdit::Normal,
      QDir::home ( ).dirName ( ), &ok );
      if ( ok && ! text.isEmpty ( ) )
    this->username = text.toStdString ( );
    }
    if ( this->username == "" ) {
      QMessageBox::critical ( this, tr ( "Error" ), tr ( "No username" ) );
      return;
    }
    this->clientMain->setUsername ( username );
    this->clientMain->connectToServer ( this->address, this->port );
  }
}
