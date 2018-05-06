#include "toolchooser.h"
#include "ui_toolchooser.h"

#include "mainwindow.h"
#include "cafbuilder.h"
#include "cafbrowser.h"
#include "quitdialog.h"

#include <QVBoxLayout>

ToolChooser::ToolChooser(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ToolChooser) {
    ui->setupUi(this);

    QObject::connect(findChild<QPushButton*>("pbtn_cafbrowse"), SIGNAL(clicked(bool)), this, SLOT(openCafBrowser()));
    QObject::connect(findChild<QPushButton*>("pbtn_cafbuild" ), SIGNAL(clicked(bool)), this, SLOT(openCafBuilder()));
    QObject::connect(findChild<QPushButton*>("pbtn_mapedit"  ), SIGNAL(clicked(bool)), this, SLOT(openMapEditor ()));

    findChild<QVBoxLayout*>("vLayout_tools")->addStretch(1);
}

void ToolChooser::openCafBuilder() {
    CafBuilder* cb = new CafBuilder;
    cb->show();
}

void ToolChooser::openCafBrowser() {
    CafBrowser* cb = new CafBrowser;
    cb->show();
}

void ToolChooser::openMapEditor() {
    MainWindow* mw = new MainWindow;
    mw->show();
}

ToolChooser::~ToolChooser() {
    delete ui;
}
