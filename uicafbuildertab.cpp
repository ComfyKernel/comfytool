#include "uicafbuildertab.h"
#include "ui_uicafbuildertab.h"

UIcafbuildertab::UIcafbuildertab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UIcafbuildertab) {
    ui->setupUi(this);
}

void UIcafbuildertab::loadFile(QString name) {
    currentFile = name;
}

UIcafbuildertab::~UIcafbuildertab() {
    delete ui;
}
