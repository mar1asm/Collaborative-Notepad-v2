#ifndef FILEFULLDIALOG_H
#define FILEFULLDIALOG_H

#include <QDialog>
#include <QMessageBox>

namespace Ui {
class FileFullDialog;
}

class FileFullDialog : public QDialog {
  Q_OBJECT

public:
  explicit FileFullDialog ( QWidget *parent = 0 );
  ~FileFullDialog ( );
  bool openFileMax ( );

private:
  Ui::FileFullDialog *ui;
};

#endif // FILEFULLDIALOG_H
