#include "fileslistdialog.h"
#include "ui_fileslistdialog.h"

FilesListDialog::FilesListDialog ( QWidget *parent )
    : QDialog ( parent ), ui ( new Ui::FilesListDialog ) {
  ui->setupUi ( this );
}

FilesListDialog::~FilesListDialog ( ) { delete ui; }

void FilesListDialog::setItems ( QVector< QPair< QString, int > > files ) {
  model = new QStringListModel ( this );
  QStringList list;
  for ( QPair< QString, int > el : files ) {
    QString fileInfo;
    fileInfo += el.first;
    fileInfo.append ( '(' );
    fileInfo += QString ( std::to_string ( el.second ).data ( ) );
    fileInfo.append ( '/' );
    fileInfo.append ( '2' );
    fileInfo.append ( ')' ); // n am gasit o varianta mai buna
    list.append ( fileInfo );
  }
  model->setStringList ( list );
  ui->listView->setModel ( model );
}
