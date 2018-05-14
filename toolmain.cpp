#include "toolmain.h"
#include "ui_toolmain.h"

#include <QCoreApplication>
#include <QXmlStreamReader>
#include <QFileDialog>
#include <QSettings>
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

    QCoreApplication::setOrganizationName  ("comfykernel");
    QCoreApplication::setOrganizationDomain("github.com/ComfyKernel/");
    QCoreApplication::setApplicationName   ("Comfy Tool");

    qRegisterMetaTypeStreamOperators<QList<QString>>("QList<QString>");
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
    n_tab->tool = this;
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

void addRecentFile(QString file) {
    QSettings settings;
    QList<QString> recent = settings.value("io/recentfiles").value<QList<QString>>();
    for(unsigned i = 0; i < recent.length(); ++i) {
        if(recent[i] == file) {
            recent.removeAt(i);
            recent.append(file);
            return;
        }
    }

    recent.append(file);
    settings.setValue("io/recentfiles", QVariant::fromValue(recent));
    settings.sync();
}

void ToolMain::openSaveDialog() {
    QString fname = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Comfy Tool Files (*.cxf)"));
    if(fname.isEmpty()) return;

    addRecentFile(fname);

    currentTab()->saveName = fname;
    saveFile();
}

void ToolMain::openFile(QString fname) {
    QFile qf(fname);

    if(!qf.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qInfo()<<"Failed opening file '"<<fname<<"' Error : "<<qf.errorString();
        return;
    }

    addRecentFile(fname);

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

void ToolMain::openFileDialog() {
    QString fname = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Comfy Tool Files (*.cxf)"));
    if(fname.isEmpty()) return;

    openFile(fname);
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