#include "toolmain.h"
#include "ui_toolmain.h"

#include <QFileDialog>

ToolMain::ToolMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ToolMain) {
    ui->setupUi(this);

    tabbar = findChild<QTabWidget*>("mainbar");
    connect(findChild<QAction*>("actionOpen"), &QAction::triggered,
            this, &ToolMain::openFileDialog);
}

ToolMain::~ToolMain() {
    delete ui;
}

void ToolMain::openTab(CTabScreen *tab) {
    tabbar->addTab(tab->makeNew(this), tab->getName());
}

void ToolMain::openDefaultTab() {
    openTab(_default_tab);
}

void ToolMain::setDefaultTab(CTabScreen *tab) {
    _default_tab = tab;
}

void ToolMain::openFileDialog() {
    QString fname = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("All Files (*.*)"));
}
