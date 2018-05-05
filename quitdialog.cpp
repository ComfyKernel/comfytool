#include "quitdialog.h"
#include "ui_quitdialog.h"

#include <QStyle>

QuitDialog::QuitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QuitDialog) {
    ui->setupUi(this);
    setSizeGripEnabled(false);
    setFixedSize(378, 74);
}

QuitDialog::~QuitDialog() {
    delete ui;
}
