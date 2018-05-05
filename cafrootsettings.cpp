#include "cafrootsettings.h"
#include "ui_cafrootsettings.h"

CafRootSettings::CafRootSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CafRootSettings) {
    ui->setupUi(this);
}

CafRootSettings::~CafRootSettings() {
    delete ui;
}
