#include "fileslistdialog.h"
#include "ui_fileslistdialog.h"

FilesListDialog::FilesListDialog ( QWidget *parent )
    : QDialog ( parent ), ui ( new Ui::FilesListDialog ) {
  ui->setupUi ( this );
  this->ui->listView->setSelectionMode ( QAbstractItemView::SingleSelection );
  this->ui->listView->setDragDropMode ( QAbstractItemView::NoDragDrop );
  this->ui->listView->setLayoutMode ( QListView::Batched );
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

void FilesListDialog::on_refreshButton_clicked ( ) {
  emit refreshListOfFiles ( );
}

void FilesListDialog::on_openButton_clicked ( ) {

  if ( ui->listView->selectionModel ( )->selectedIndexes ( ).size ( ) == 0 ) {
    QMessageBox::critical ( this, tr ( "Invalid selection" ),
                "No file selected" );
    return;
  }
  QString filename;
  int fileIndex;

  QModelIndex tempIndex = ui->listView->currentIndex ( );
  fileIndex = tempIndex.row ( );

  filename = tempIndex.data ( Qt::DisplayRole ).toString ( );
  emit openFile ( fileIndex, filename );
}

void FilesListDialog::on_deleteButton_clicked ( ) {
  if ( ui->listView->selectionModel ( )->selectedIndexes ( ).size ( ) == 0 ) {
    QMessageBox::critical ( this, tr ( "Invalid selection" ),
                "No file selected" );
    return;
  }
  QString filename;
  int fileIndex;
  QModelIndex tempIndex = ui->listView->currentIndex ( );
  fileIndex = tempIndex.row ( );

  filename = tempIndex.data ( Qt::DisplayRole ).toString ( );
  emit deleteFile ( fileIndex, filename );
}
