#include "uicafbuildertab.h"
#include "ui_uicafbuildertab.h"

#include "uicafbuilderrootinfo.h"
#include "uicafbuildertablumpinfo.h"

#include <QXmlStreamWriter>
#include <QGridLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDebug>
#include <QFrame>

UIcafbuildertab::UIcafbuildertab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UIcafbuildertab) {
    ui->setupUi(this);
    buildWidget = findChild<QListWidget*>("lw_lumps");
    connect(buildWidget, SIGNAL(currentRowChanged(int)), this, SLOT(updateInfoPanel(int)));
}

void addRoot(UIcafbuildertab* ucb) {
    ucb->buildWidget->addItem("[ROOT]");
}

void UIcafbuildertab::loadFile(QString name) {
    currentFile = name;
    addRoot(this);
}

void UIcafbuildertab::parseCXF(QString name, QXmlStreamReader *qxsr) {
    currentFile = name;
    addRoot(this);

    while(!qxsr->atEnd() && !qxsr->hasError()) {
        QXmlStreamReader::TokenType qtt = qxsr->readNext();

        if(qtt == QXmlStreamReader::StartDocument) {
            continue;
        }

        if(qtt == QXmlStreamReader::StartElement) {
            if(qxsr->name() == "info") {
                QXmlStreamAttributes qxsa = qxsr->attributes();
                for(const QXmlStreamAttribute& a : qxsa) {
                    if(a.name() == "path") {
                        rootInfo.path = a.value().toString();
                        continue;
                    }

                    if(a.name() == "major") {
                        rootInfo.cvMajor = a.value().toUInt();
                        continue;
                    }

                    if(a.name() == "minor") {
                        rootInfo.cvMinor = a.value().toInt();
                        continue;
                    }

                    if(a.name() == "revision") {
                        rootInfo.revision = a.value().toInt();
                        continue;
                    }
                }
            }

            if(qxsr->name() == "lumpitem") {
                Lump l;
                QXmlStreamAttributes qxsa = qxsr->attributes();
                for(const QXmlStreamAttribute& a : qxsa) {
                    if(a.name() == "name") {
                        l.name = a.value().toString();
                        continue;
                    }
                    if(a.name() == "path") {
                        l.path = a.value().toString();
                        continue;
                    }
                    if(a.name() == "type") {
                        l.type = a.value().toString();
                        continue;
                    }
                    if(a.name() == "data") {
                        l.datapath = a.value().toString();
                        continue;
                    }
                    if(a.name() == "revision") {
                        l.revision = a.value().toUInt();
                        continue;
                    }
                    if(a.name() == "link") {
                        l.link = a.value().toUInt();
                    }
                }
                addVisItem(QString("[LUMP] ") + l.path + l.name, l);
            }
        }
    }

    if(qxsr->hasError()) {
        QMessageBox::critical(this, "Failed parsing CXF", qxsr->errorString(), QMessageBox::Ok);
        return;
    }

    buildWidget->itemAt(0, 0)->setText(QString("[ROOT] ") + rootInfo.path);
}

void UIcafbuildertab::saveFile(QString name) {
    if(name.isEmpty()) return;

    QFile qf(name);
    if(!qf.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qInfo()<<"Failed to open file '"<<name<<"' for saving ("<<qf.errorString()<<")";
        return;
    }

    currentFile = name;
    saveset     = true;

    _ct->mainTabBar->setTabText(_ct->mainTabBar->indexOf(this), QString("Builder : ") + QString(currentFile).remove(0, currentFile.lastIndexOf('/') + 1));

    QXmlStreamWriter qsw(&qf);
    qsw.setAutoFormatting(true);
    qsw.setAutoFormattingIndent(true);

    qsw.writeStartDocument();
    qsw.writeStartElement("root");
    qsw.writeAttribute("type", "cafbuilder");

    qsw.writeStartElement("info");
    qsw.writeAttribute("path"    , rootInfo.path);
    qsw.writeAttribute("major"   , QString::number(rootInfo.cvMajor ));
    qsw.writeAttribute("minor"   , QString::number(rootInfo.cvMinor ));
    qsw.writeAttribute("revision", QString::number(rootInfo.revision));
    qsw.writeEndElement();

    for(const Lump& l : lumps) {
        qsw.writeStartElement("lumpitem");

        qsw.writeAttribute("name", l.name);
        qsw.writeAttribute("path", l.path);
        qsw.writeAttribute("type",l.type);
        qsw.writeAttribute("data", l.datapath);
        qsw.writeAttribute("revision", QString::number(l.revision));

        if(l.link) {
            qsw.writeAttribute("link", "1");
        }

        qsw.writeEndElement();
    }

    qsw.writeEndElement();
    qf.close();
}

unsigned currentLump(UIcafbuildertab* cbt) {
    return cbt->buildWidget->currentRow() - 1;
}

void UIcafbuildertab::updateInfoPanel(int current) {
    if(panelWidget != nullptr) {
        //qInfo()<<panelWidget->metaObject()->className();
        if(QString(panelWidget->metaObject()->className()) == "UIcafbuilderrootinfo") {
            //qInfo()<<"Is Root Info";
            applyRootSettings();
        } else if(QString(panelWidget->metaObject()->className()) == "UIcafbuildertablumpinfo") {
            //qInfo()<<"Is Lump Info";
            applyLumpSettings(currLump);
        }

        findChild<QGridLayout*>("gl_panel")->removeWidget(panelWidget);
        panelWidget->close();
        delete panelWidget;
    }

    if(!current) {
        UIcafbuilderrootinfo* cbri = new UIcafbuilderrootinfo(findChild<QFrame*>("fr_panel"));
        findChild<QGridLayout*>("gl_panel")->addWidget(cbri, 0, 0, 1, 1, Qt::Alignment());

        cbri->findChild<QSpinBox* >("sb_cvMajor" )->setValue(rootInfo.cvMajor);
        cbri->findChild<QSpinBox* >("sb_cvMinor" )->setValue(rootInfo.cvMinor);
        cbri->findChild<QSpinBox* >("sb_revision")->setValue(rootInfo.revision);
        cbri->findChild<QLineEdit*>("le_path"    )->setText (rootInfo.path);

        connect(cbri->findChild<QSpinBox* >("sb_cvMajor" ), SIGNAL(valueChanged(  int  )), this, SLOT(setUnsaved()));
        connect(cbri->findChild<QSpinBox* >("sb_cvMinor" ), SIGNAL(valueChanged(  int  )), this, SLOT(setUnsaved()));
        connect(cbri->findChild<QSpinBox* >("sb_revision"), SIGNAL(valueChanged(  int  )), this, SLOT(setUnsaved()));
        connect(cbri->findChild<QLineEdit*>("le_path"    ), SIGNAL(textChanged (QString)), this, SLOT(setUnsaved()));

        connect(cbri->findChild<QPushButton*>("pbtn_reset"), SIGNAL(clicked(bool)), this, SLOT(resetRootSettings()));

        panelWidget = cbri;
        cbri->show();
        return;
    }

    UIcafbuildertablumpinfo* cbli = new UIcafbuildertablumpinfo(findChild<QFrame*>("fr_panel"));
    findChild<QGridLayout*>("gl_panel")->addWidget(cbli, 0, 0, 1, 1, Qt::Alignment());

    currLump = currentLump(this);

    cbli->findChild<QLineEdit*>("le_name")->setText(lumps[currentLump(this)].name);
    cbli->findChild<QLineEdit*>("le_path")->setText(lumps[currentLump(this)].path);
    cbli->findChild<QLineEdit*>("le_type")->setText(lumps[currentLump(this)].type);
    cbli->findChild<QLineEdit*>("le_data")->setText(lumps[currentLump(this)].datapath);
    cbli->findChild<QSpinBox*>("sb_revision")->setValue(lumps[currentLump(this)].revision);
    cbli->findChild<QCheckBox*>("cb_lumplink")->setChecked(lumps[currentLump(this)].link);

    connect(cbli->findChild<QLineEdit*>("le_name"), SIGNAL(textChanged(QString)), this, SLOT(setUnsaved()));
    connect(cbli->findChild<QLineEdit*>("le_path"), SIGNAL(textChanged(QString)), this, SLOT(setUnsaved()));
    connect(cbli->findChild<QLineEdit*>("le_type"), SIGNAL(textChanged(QString)), this, SLOT(setUnsaved()));
    connect(cbli->findChild<QLineEdit*>("le_data"), SIGNAL(textChanged(QString)), this, SLOT(setUnsaved()));
    connect(cbli->findChild<QSpinBox*>("sb_revision"), SIGNAL(valueChanged(int)), this, SLOT(setUnsaved()));
    connect(cbli->findChild<QPushButton*>("pbtn_reset"), SIGNAL(clicked(bool)), this, SLOT(resetLumpSettings()));
    connect(cbli->findChild<QCheckBox*>("cb_lumpflag"), SIGNAL(toggled(bool)), this, SLOT(setUnsaved()));

    cbli->show();
    panelWidget = cbli;
}

void UIcafbuildertab::applyRootSettings() {
    UIcafbuilderrootinfo* cbri = (UIcafbuilderrootinfo*)panelWidget;
    rootInfo.cvMajor  = cbri->findChild<QSpinBox*>("sb_cvMajor")->value();
    rootInfo.cvMinor  = cbri->findChild<QSpinBox*>("sb_cvMinor")->value();
    rootInfo.revision = cbri->findChild<QSpinBox*>("sb_revision")->value();
    rootInfo.path     = cbri->findChild<QLineEdit*>("le_path")->text();
}

void UIcafbuildertab::applyLumpSettings(int lump) {
    UIcafbuildertablumpinfo* cbli = (UIcafbuildertablumpinfo*)panelWidget;
    lumps[lump].name = cbli->findChild<QLineEdit*>("le_name")->text();
    lumps[lump].path = cbli->findChild<QLineEdit*>("le_path")->text();
    lumps[lump].type = cbli->findChild<QLineEdit*>("le_type")->text();
    lumps[lump].datapath = cbli->findChild<QLineEdit*>("le_data")->text();
    lumps[lump].revision = cbli->findChild<QSpinBox*> ("sb_revision")->value();
    lumps[lump].link     = cbli->findChild<QCheckBox*>("cb_lumplink")->isChecked();
}

void UIcafbuildertab::resetRootSettings() {
    UIcafbuilderrootinfo* cbri = (UIcafbuilderrootinfo*)panelWidget;
    cbri->findChild<QSpinBox*> ("sb_cvMajor" )->setValue(rootInfo.cvMajor);
    cbri->findChild<QSpinBox*> ("sb_cvMinor" )->setValue(rootInfo.cvMinor);
    cbri->findChild<QSpinBox*> ("sb_revision")->setValue(rootInfo.revision);
    cbri->findChild<QLineEdit*>("le_path"    )->setText (rootInfo.path);
}

void UIcafbuildertab::resetLumpSettings() {
    UIcafbuildertablumpinfo* cbli = (UIcafbuildertablumpinfo*)panelWidget;
    cbli->findChild<QLineEdit*>("le_name")->setText(lumps[currentLump(this)].name);
    cbli->findChild<QLineEdit*>("le_path")->setText(lumps[currentLump(this)].path);
    cbli->findChild<QLineEdit*>("le_type")->setText(lumps[currentLump(this)].type);
    cbli->findChild<QLineEdit*>("le_data")->setText(lumps[currentLump(this)].datapath);
    cbli->findChild<QSpinBox*>("sb_revision")->setValue(lumps[currentLump(this)].revision);
    cbli->findChild<QCheckBox*>("cb_lumplink")->setChecked(lumps[currentLump(this)].link);
}

void UIcafbuildertab::addVisItem(QString name, Lump l) {
    lumps.append(l);
    buildWidget->addItem(name);
}

void UIcafbuildertab::setUnsaved() {
    _ct->mainTabBar->setTabText(_ct->mainTabBar->indexOf(this), QString("Builder : ") + QString(currentFile).remove(0, currentFile.lastIndexOf('/') + 1) + "*");
}

void UIcafbuildertab::openSaveDialog(bool skipdialog) {
    if(!saveset || !skipdialog) {
        QString fname = QFileDialog::getSaveFileName(this, tr("Save CXF File"), "", tr("Comfy XML File (*.cxf)"));
        if(fname.isEmpty()) return;
        saveFile(fname);
    } else {
        saveFile(currentFile);
    }
}

UIcafbuildertab::~UIcafbuildertab() {
    delete ui;
}
