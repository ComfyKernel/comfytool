#include "toolmain.h"
#include "ui_toolmain.h"

#include <QXmlStreamReader>
#include <QFileDialog>
#include <QMenuBar>
#include <QDebug>
#include <QFile>

ToolMain::ToolMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ToolMain) {
    ui->setupUi(this);

    tabbar = findChild<QTabWidget*>("mainbar");

    connect(findChild<QAction*>("actionOpen"), &QAction::triggered,
            this, &ToolMain::openFileDialog);
    connect(findChild<QAction*>("actionSave"), &QAction::triggered,
            this, &ToolMain::saveFile);
    connect(findChild<QAction*>("actionSave_As"), &QAction::triggered,
            this, &ToolMain::openSaveDialog);

    connect(tabbar, &QTabWidget::currentChanged,
            this  , &ToolMain::onTabChanged);
    connect(tabbar, &QTabWidget::tabCloseRequested,
            this, &ToolMain::closeTab);
}

ToolMain::~ToolMain() {
    delete ui;
}

void ToolMain::addFileTypeRef(CTabScreen *ref, const QString &name) {
    _refpairs.append(_refpair(ref, name));
}

void ToolMain::removeFileTypeRef(const QString &name) {
    for(int i = 0; i < _refpairs.length(); ++i) {
        if(_refpairs[i].second == name) {
            _refpairs.removeAt(i);
            return;
        }
    }

    qInfo()<<"Unable to find ref '"<<name<<"'";
}

CTabScreen* ToolMain::openTab(CTabScreen *tab) {
    CTabScreen* n_tab = tab->makeNew(this);
    tabbar->addTab(n_tab, n_tab->getName());
    return n_tab;
}

void ToolMain::openDefaultTab() {
    openTab(_default_tab);
}

void ToolMain::setDefaultTab(CTabScreen *tab) {
    _default_tab = tab;
}

void ToolMain::saveFile() {
    if(currentTab()->saveName.isEmpty()) {
        openSaveDialog();
        return;
    }

    currentTab()->saveFile(currentTab()->saveName);
}

void ToolMain::openSaveDialog() {
    QString fname = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Comfy Tool Files (*.cxf)"));
    if(fname.isEmpty()) return;
    currentTab()->saveName = fname;
    saveFile();
}

void ToolMain::openFileDialog() {
    QString fname = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Comfy Tool Files (*.cxf)"));
    if(fname.isEmpty()) return;

    QFile qf(fname);

    if(!qf.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qInfo()<<"Failed opening file '"<<fname<<"' Error : "<<qf.errorString();
        return;
    }

    QXmlStreamReader qx(&qf);

    while(!qx.atEnd() && !qx.hasError()) {
        QXmlStreamReader::TokenType token = qx.readNext();
        switch(token) {
        case QXmlStreamReader::StartDocument:
            continue;
        case QXmlStreamReader::StartElement:
            if(qx.name() == "root") {
                QXmlStreamAttributes attrs = qx.attributes();
                for(const QXmlStreamAttribute& a : attrs) {
                    if(a.name() == "type") {
                        QString tp = a.value().toString();

                        for(const auto& i : _refpairs) {
                            if(i.second == tp) {
                                CTabScreen* tab = openTab(i.first);
                                tab->loadFile(fname);
                                qf.close();
                                return;
                            }
                        }
                    }
                }
            }
            continue;
        default:
            continue;
        }
    }

    qf.close();
}

void ToolMain::onTabChanged(int tab) {
    QMenuBar* mb = findChild<QMenuBar*>("menubar");
    CTabScreen* cts = (CTabScreen*)tabbar->widget(tab);
    if(!cts) {
        return;
    }

    QList<QObject*> mb_ch = mb->children();
    for(int i = 0; i < mb_ch.length(); ++i) {
        if(QString(mb_ch[i]->metaObject()->className()) == "QMenu") {
            QMenu* m = (QMenu*)mb_ch[i];
            QString name = m->objectName();
            if(name == "menuFile" || name == "menuWindow") {
                continue;
            }
            m->close();
            delete m;
        }
    }

    QList<QMenu*> menus = cts->menus(mb);

    for(QMenu* m : menus) {
        mb->addMenu(m);
    }
}

void ToolMain::closeTab(int tab) {
    CTabScreen* cts = (CTabScreen*)tabbar->widget(tab);
    cts->close();
    tabbar->removeTab(tab);
}

CTabScreen* ToolMain::currentTab() {
    return (CTabScreen*)tabbar->currentWidget();
}
