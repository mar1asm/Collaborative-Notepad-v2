#include "clientwindow.h"
#include "ui_clientwindow.h"

ClientWindow::ClientWindow ( QWidget *parent )
    : QMainWindow ( parent ), ui ( new Ui::ClientWindow ) {
  ui->setupUi ( this );
  this->setCentralWidget ( ui->textEdit );
  setWindowTitle ( "New file" );
  clientMain = new ClientMain ( this );
}

int ClientWindow::setConnectionData ( ) {
  if ( ! QFile::exists ( ":/files/config" ) ) {
    QMessageBox::critical ( this, tr ( "Error" ),
                tr ( "missing config file" ) );
    return -1;
  }

  QFile file ( ":/files/config" );
  if ( ! file.open ( QIODevice::ReadOnly | QIODevice::Text ) ) {
    QMessageBox::critical ( this, tr ( "Error" ),
                tr ( "cant open config file" ) );
    return -1;
  }
  QString data;
  data = file.readAll ( );
  std::stringstream data2 ( data.toStdString ( ) ); // workaround
  std::string line;
  data2 >> line;
  int addressLength = line.size ( );
  address = new char[ addressLength + 1 ];
  strcpy ( address, line.data ( ) );
  address[ addressLength ] = '\0';
  data2 >> line;
  port = atoi ( line.data ( ) );
  return 0;
}

ClientWindow::~ClientWindow ( ) {
  if ( connected )
    this->clientMain->sendRequest ( { "quit" } );
  delete ui;
}

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

void ClientWindow::on_actionFrom_server_triggered ( ) {
  clientMain->sendRequest ( { "list" } );
}

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
    this->connected = false;
    this->ui->actionToggleConnection->setIcon ( QIcon ( ":/icons/connect" ) );
    this->clientMain->sendRequest ( { "quit" } );
    this->ui->actionFrom_server->setDisabled ( true );
    QMessageBox::information ( this, tr ( "Server" ),
                   tr ( "Disconnected from server" ) );
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
    if ( this->clientMain->connectToServer ( this->address, this->port ) ) {
      if ( this->username == "" ) {
    QMessageBox::critical ( this, tr ( "Error" ),
                tr ( "Cannot connect to server" ) );
    return;
      }
    } else {
      QMessageBox::information ( this, tr ( "Server" ),
                 tr ( "Connected to server" ) );
      this->connected = true;
      this->ui->actionToggleConnection->setIcon (
      QIcon ( ":/icons/disconnect" ) );
      this->ui->actionFrom_server->setEnabled ( true );
    }
  }
}
