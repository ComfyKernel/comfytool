#include "toolchooser.h"
#include "ui_toolchooser.h"

#include <QVBoxLayout>

ToolChooser::ToolChooser(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ToolChooser) {
    ui->setupUi(this);

    QVBoxLayout* qvb = findChild<QVBoxLayout*>("vLayout_tools");
    qvb->addStretch(1);
}

ToolChooser::~ToolChooser() {
    delete ui;
}
