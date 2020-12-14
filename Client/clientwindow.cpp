#include "clientwindow.h"
#include "ui_clientwindow.h"

ClientWindow::ClientWindow ( QWidget *parent )
    : QMainWindow ( parent ), ui ( new Ui::ClientWindow ) {
  ui->setupUi ( this );
  this->setCentralWidget ( ui->textEdit );
  setWindowTitle ( "New file" );
  clientMain = new ClientMain ( this );
  connect ( clientMain, &ClientMain::serverClosed, this,
        &ClientWindow::on_serverClosed );
  connect ( clientMain, &ClientMain::logMessage, this,
        &ClientWindow::sendMessage );
  connect ( clientMain, &ClientMain::openDialog, this,
        &ClientWindow::on_openDialog );
  connect ( clientMain, &ClientMain::closeDialog, this,
        &ClientWindow::on_closeDialog );
  connect ( clientMain, &ClientMain::addLine, this, &ClientWindow::on_addLine );
}

int ClientWindow::setConnectionData ( ) {
  if ( ! QFile::exists ( ":/files/config" ) ) {
    sendMessage ( "critical", "Error", "Cannot find config file " );
    return -1;
  }

  QFile file ( ":/files/config" );
  if ( ! file.open ( QIODevice::ReadOnly | QIODevice::Text ) ) {
    sendMessage ( "critical", "Error",
          "Cannot open config file : " +
              file.errorString ( ).toStdString ( ) );
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
    sendMessage ( "warning", "Warning",
          "Cannot open file : " +
              savedFile.errorString ( ).toStdString ( ) );
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
    sendMessage ( "warning", "Warning",
          "Cannot open file : " +
              file.errorString ( ).toStdString ( ) );
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
    sendMessage ( "warning", "Warning",
          "Cannot save file : " +
              file.errorString ( ).toStdString ( ) );
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
    sendMessage ( "warning", "Warning",
          "Cannot save file : " +
              file.errorString ( ).toStdString ( ) );
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
    sendMessage ( "info", "server", "Disconnected from server" );
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
      sendMessage ( "critical", "Error", "no username" );
      return;
    }
    this->clientMain->setUsername ( username );
    if ( this->clientMain->connectToServer ( this->address, this->port ) ) {
      if ( this->username == "" ) {
    sendMessage ( "critical", "Error", "Cannot connect to server" );
    return;
      }
    } else {
      sendMessage ( "info", "Server", "Connected to server" );
      this->connected = true;
      this->ui->actionToggleConnection->setIcon (
      QIcon ( ":/icons/disconnect" ) );
      this->ui->actionFrom_server->setEnabled ( true );
    }
  }
}

void ClientWindow::on_serverClosed ( ) {

  this->connected = false;
  this->ui->actionToggleConnection->setIcon ( QIcon ( ":/icons/connect" ) );
  this->ui->actionFrom_server->setDisabled ( true );
  clientMain->closeConnection ( );
  sendMessage ( "info", "Disconnected from server",
        "The server encountered an internal error" );
}

void ClientWindow::sendMessage ( std::string type, std::string sub,
                 std::string message ) {
  if ( type == "info" )
    QMessageBox::information ( this, tr ( sub.data ( ) ),
                   tr ( message.data ( ) ) );
  else if ( type == "critical" )
    QMessageBox::critical ( this, tr ( sub.data ( ) ),
                tr ( message.data ( ) ) );
  else
    QMessageBox::warning ( this, tr ( sub.data ( ) ), tr ( message.data ( ) ) );
}

void ClientWindow::on_openDialog ( QVector< QPair< QString, int > > files ) {
  std::cout << "am ajuns aici"
        << "\n";
  filesListDialog = new FilesListDialog ( this );
  connect ( filesListDialog, &FilesListDialog::refreshListOfFiles, clientMain,
        &ClientMain::on_refreshFiles );
  connect ( filesListDialog, &FilesListDialog::deleteFile, clientMain,
        &ClientMain::on_deleteFile );
  connect ( filesListDialog, &FilesListDialog::openFile, this,
        &ClientWindow::on_openFile );
  filesListDialog->setItems ( files );
  filesListDialog->show ( );
}

void ClientWindow::on_closeDialog ( ) {
  delete filesListDialog;
  filesListDialog = nullptr;
}

void ClientWindow::on_openFile ( int fileId, QString filename ) {
  std::string editedFilename;
  editedFilename = filename.toStdString ( );
  editedFilename =
      editedFilename.substr ( 0, editedFilename.find_last_of ( ' ' ) );
  this->currentFile = editedFilename.data ( );
  setWindowTitle ( editedFilename.data ( ) );
  clientMain->on_OpenFile ( fileId, editedFilename.data ( ) );
  this->ui->textEdit->setText ( "" );
}

void ClientWindow::on_addLine ( QString line ) {
  this->ui->textEdit->append ( line );
}
