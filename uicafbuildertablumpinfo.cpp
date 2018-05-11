#include "uicafbuildertablumpinfo.h"
#include "ui_uicafbuildertablumpinfo.h"

#include <QFileDialog>

UIcafbuildertablumpinfo::UIcafbuildertablumpinfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UIcafbuildertablumpinfo) {
    ui->setupUi(this);
    connect(findChild<QPushButton*>("pbtn_open"), SIGNAL(clicked(bool)), this, SLOT(openFileSearch()));

    findChild<QVBoxLayout*>("vl_flag1")->addStretch();
    findChild<QVBoxLayout*>("vl_flag2")->addStretch();
}

void UIcafbuildertablumpinfo::openFileSearch() {
    QString fname = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("All Files (*.*)"));
    if(!fname.isEmpty()) {
        findChild<QLineEdit*>("le_data")->setText(fname);
        return;
    }
}

UIcafbuildertablumpinfo::~UIcafbuildertablumpinfo() {
    delete ui;
}
