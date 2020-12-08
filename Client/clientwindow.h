#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <QMainWindow>

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QShortcut>
#include <QTextStream>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>

namespace Ui {
class ClientWindow;
}

class ClientWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit ClientWindow ( QWidget *parent = 0 );
  ~ClientWindow ( );

private slots:
  void on_actionNew_triggered ( );

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

private:
  Ui::ClientWindow *ui;
  QString currentFile = "";
  bool helper_isSaved ( );
};

#endif // CLIENTWINDOW_H
