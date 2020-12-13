#ifndef FILESLISTDIALOG_H
#define FILESLISTDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QPair>
#include <QStringListModel>
#include <stdio.h>
#include <stdlib.h>

namespace Ui {
class FilesListDialog;
}

class FilesListDialog : public QDialog {
  Q_OBJECT

public:
  explicit FilesListDialog ( QWidget *parent = 0 );
  ~FilesListDialog ( );
  void setItems ( QVector< QPair< QString, int > > files );

private slots:
  void on_refreshButton_clicked ( );

  void on_openButton_clicked ( );

  void on_deleteButton_clicked ( );

private:
  Ui::FilesListDialog *ui;
  QStringListModel *model;

signals:
  void refreshListOfFiles ( );
  void deleteFile ( int fileId, QString filename );
  void openFile ( int fileId, QString filename );
};

#endif // FILESLISTDIALOG_H
