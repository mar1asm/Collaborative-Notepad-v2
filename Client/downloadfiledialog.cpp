#include "downloadfiledialog.h"
#include "ui_downloadfiledialog.h"

DownloadFileDialog::DownloadFileDialog ( QWidget *parent )
    : QDialog ( parent ), ui ( new Ui::DownloadFileDialog ) {
  ui->setupUi ( this );
  this->ui->listView->setSelectionMode ( QAbstractItemView::SingleSelection );
  this->ui->listView->setDragDropMode ( QAbstractItemView::NoDragDrop );
  this->ui->listView->setLayoutMode ( QListView::Batched );
}

void DownloadFileDialog::setItems ( QVector< QString > files ) {
  model = new QStringListModel ( this );

  model->setStringList ( files.toList ( ) );
  ui->listView->setModel ( model );
}

DownloadFileDialog::~DownloadFileDialog ( ) { delete ui; }

void DownloadFileDialog::on_downloadButton_clicked ( ) {
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
  emit downloadFile ( filename );
}

void DownloadFileDialog::on_cancelButton_clicked ( ) { emit cancel ( ); }
