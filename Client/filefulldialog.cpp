#include "filefulldialog.h"
#include "ui_filefulldialog.h"

FileFullDialog::FileFullDialog ( QWidget *parent )
    : QDialog ( parent ), ui ( new Ui::FileFullDialog ) {
  ui->setupUi ( this );
}

bool FileFullDialog::openFileMax ( ) {
  QMessageBox editOnly ( this );
  editOnly.setText (
      "Fisierul este deja editat de 2 persoane. Poti sa il deschizi "
      "insa modificarile facute nu vor modifica fisierul pe server" );
  editOnly.setInformativeText ( "Doresti sa l deschizi totusi?" );
  editOnly.setStandardButtons ( QMessageBox::Open | QMessageBox::Cancel );
  editOnly.setDefaultButton ( QMessageBox::Open );
  int answer = editOnly.exec ( );
  switch ( answer ) {
  case QMessageBox::Open:
    return true;
  case QMessageBox::Cancel:
    return false;
  default:
    break;
  }
}

FileFullDialog::~FileFullDialog ( ) { delete ui; }
