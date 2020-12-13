#ifndef FILESLISTDIALOG_H
#define FILESLISTDIALOG_H

#include <QDialog>
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

private:
  Ui::FilesListDialog *ui;
  QStringListModel *model;
};

#endif // FILESLISTDIALOG_H
