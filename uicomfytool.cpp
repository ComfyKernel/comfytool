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
    connect(findChild<QAction*>("actionSave"), SIGNAL(triggered(bool)), this, SLOT(openSaveDialog()));
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

void UIcomfytool::openSaveDialog() {
    QString wname = mainTabBar->widget(mainTabBar->currentIndex())->metaObject()->className();
    if(wname == "UIcafbuildertab") {
        UIcafbuildertab* cbt = (UIcafbuildertab*)mainTabBar->widget(mainTabBar->currentIndex());
        cbt->openSaveDialog();
        return;
    }
}

void UIcomfytool::openHomeTab() {
    UIhometab* ht = new UIhometab();
    connect(ht->findChild<QPushButton*>("pbtn_homeOpenFile"), SIGNAL(clicked(bool)), findChild<QAction*>("actionOpen"), SLOT(trigger()));
    addMainbarTab("Home", ht);
}

void UIcomfytool::openCAFBuilder(QString name, QXmlStreamReader* qsr) {
    if(name.isEmpty() && qsr == nullptr) {
        QMessageBox::critical(this, "Error opening CAF Builder", "File name ref and XML Parser are null", QMessageBox::Ok);
        return;
    }
    UIcafbuildertab* cafbuilder = new UIcafbuildertab();
    cafbuilder->_ct = this;

    QString tabName = name;
    tabName.remove(0, tabName.lastIndexOf('/') + 1);

    addMainbarTab(QString("Builder : ") + tabName, cafbuilder);
    mainTabBar->setCurrentIndex(mainTabBar->count() - 1);

    if(qsr == nullptr) {
        cafbuilder->loadFile(name);
        return;
    }

    cafbuilder->parseCXF(name, qsr);
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

    if(ext == ".cxf") {
        QFile* qf = new QFile(fname);

        if(!qf->open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::critical(this, "Failed loading CXF!", QString("Failed to open CXF (xml) file '") + fname + "'", QMessageBox::Ok);
            return;
        }

        QXmlStreamReader* qxsr = new QXmlStreamReader(qf);
        qInfo()<<"Opening CXF (Comfy XML File)";

        qInfo()<<"Finding CXF Type";
        while(!qxsr->atEnd() && !qxsr->hasError()) {
            QXmlStreamReader::TokenType token = qxsr->readNext();

            if(token == QXmlStreamReader::StartDocument) {
                continue;
            }

            if(token == QXmlStreamReader::StartElement) {
                if(qxsr->name() == "root") {
                    QXmlStreamAttributes attrs = qxsr->attributes();
                    for(const QXmlStreamAttribute& a : attrs) {
                        if(a.name() == "type") {
                            qInfo()<<"Type found";
                            if(a.value() == "cafbuilder") {
                                openCAFBuilder(fname, qxsr);
                                return;
                            }
                        }
                    }
                }
            }
        }

        if(qxsr->hasError()) {
            QMessageBox::critical(this, "CXF XML Parsing Error", QString(qxsr->errorString()), QMessageBox::Ok);
            return;
        }

        QMessageBox::critical(this, "Unable to find type", QString("Cannot decipher type for file '") + fname + "'. Check for missing attribute 'type' on root.", QMessageBox::Ok);
        return;
    }
}

UIcomfytool::~UIcomfytool() {
    delete ui;
}
