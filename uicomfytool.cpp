#include "uicomfytool.h"
#include "ui_uicomfytool.h"

#include "uihometab.h"
#include "uicafbuildertab.h"

#include <QMessageBox>
#include <QPushButton>
#include <QAction>
#include <QDebug>
#include <QLabel>
#include <QFileDialog>

UIcomfytool::UIcomfytool(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UIcomfytool) {
    ui->setupUi(this);

    mainTabBar = findChild<QTabWidget*>("main_tabbar");
    openHomeTab();

    connect(mainTabBar, SIGNAL(tabCloseRequested(int)), this, SLOT(removeMainbarTab(int)));

    connect(findChild<QAction*>("actionOpen"), SIGNAL(triggered(bool)), this, SLOT(openFileDialog()));
}

void UIcomfytool::closeEvent(QCloseEvent* event) {
    QMessageBox exitMsg(this);
    exitMsg.setText("<h3>Are you sure you want to quit?</h3>");
    exitMsg.setInformativeText("<p align='center'>Files may not be saved.</p>");
    exitMsg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    exitMsg.setDefaultButton(QMessageBox::No);

    int ret = exitMsg.exec();

    if(ret == QMessageBox::Yes) {
        event->accept();
        return;
    }

    event->ignore();
}

void UIcomfytool::removeMainbarTab(int tab) {
    mainTabBar->removeTab(tab);
    if(mainTabBar->count() < 1) {
        openHomeTab();
    }
}

void UIcomfytool::addMainbarTab(QString name, QWidget* qw) {
    mainTabBar->addTab(qw, name);
}

void UIcomfytool::openHomeTab() {
    UIhometab* ht = new UIhometab();
    connect(ht->findChild<QPushButton*>("pbtn_homeOpenFile"), SIGNAL(clicked(bool)), findChild<QAction*>("actionOpen"), SLOT(trigger()));
    addMainbarTab("Home", ht);
}

void UIcomfytool::openCAFBuilder(QString name) {
    UIcafbuildertab* cafbuilder = new UIcafbuildertab();
    QString tabName = name;
    tabName.remove(0, tabName.lastIndexOf('/') + 1);
    addMainbarTab(QString("Builder : ") + tabName, cafbuilder);
    cafbuilder->loadFile(name);
}

void UIcomfytool::openFileDialog() {
    QString fname = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::homePath(), tr("All Files (*.*)"));
    if(fname.isEmpty()) {
        qInfo()<<"File Dialog Skipped";
        return;
    }
    QString ext = fname;
    ext.remove(0, ext.lastIndexOf('.'));

    qInfo()<<"Opening file '"<<fname<<"' (extension '"<<ext<<"')";

    if(ext == ".cbf") {
        qInfo()<<"Opening CBF (Comfy Builder File)";
        openCAFBuilder(fname);
        return;
    }
}

UIcomfytool::~UIcomfytool() {
    delete ui;
}
