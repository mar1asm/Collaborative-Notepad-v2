#ifndef DOWNLOADFILEDIALOG_H
#define DOWNLOADFILEDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QStringListModel>

namespace Ui {
class DownloadFileDialog;
}

class DownloadFileDialog : public QDialog {
  Q_OBJECT

public:
  explicit DownloadFileDialog ( QWidget *parent = 0 );
  void setItems ( QVector< QString > files );
  ~DownloadFileDialog ( );

private slots:

  void on_downloadButton_clicked ( );

  void on_cancelButton_clicked ( );

private:
  Ui::DownloadFileDialog *ui;
  QStringListModel *model;

signals:
  void downloadFile ( QString filename );
  void cancel ( );
};

#endif // DOWNLOADFILEDIALOG_H
