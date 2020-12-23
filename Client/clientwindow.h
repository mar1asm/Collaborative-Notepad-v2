#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include "clientmain.h"
#include "fileslistdialog.h"

#include "downloadfiledialog.h"
#include <QFile>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QPair>
#include <QShortcut>
#include <QTextEdit>
#include <QTextStream>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>
#include <fstream>
#include <iostream>
#include <sstream>

namespace Ui {
class ClientWindow;
}

class ClientWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit ClientWindow ( QWidget *parent = 0 );
  int setConnectionData ( );
  ~ClientWindow ( );

private slots:
  void on_actionSave_triggered ( );

  void on_actionSave_as_triggered ( );

  void on_actionExit_triggered ( );

  void on_actionCopy_triggered ( );

  void on_actionPaste_triggered ( );

  void on_actionUndo_triggered ( );

  void on_actionRedo_triggered ( );

  void on_actionCut_triggered ( );

  void on_actionFrom_server_triggered ( );

  void on_actionFrom_PC_triggered ( );

  void on_actionToggleConnection_triggered ( );

  void on_serverClosed ( );

  void sendMessage ( std::string type, std::string sub, std::string message );

  void on_openDialog ( QVector< QPair< QString, int > > files );

  void on_closeDialog ( );

  void on_openFile ( int fileId, QString filename );

  void on_addLine ( QString line );

  void on_actionDownload_triggered ( );

  void on_openDownloadDialog ( QVector< QString > files );

  void on_closeDownloadDialog ( );

  void on_downloadFile ( QString filename );

  void on_downloadSetName ( QString fileContent );

  void on_actionNewLocal_triggered ( );

  void on_actionNewRemote_triggered ( );

  void on_actionUpload_triggered ( );

private:
  Ui::ClientWindow *ui;
  QString currentFile = "";
  bool helper_isSaved ( );
  bool connected = false;
  int port;
  char *address;
  ClientMain *clientMain;
  std::string username = "";

  FilesListDialog *filesListDialog;
  DownloadFileDialog *downloadDialog;

  void getUsername ( );

  bool locally = true;
signals:
  void textEdited ( int pos, int addedChars, int removedChars );
};

#endif // CLIENTWINDOW_H
