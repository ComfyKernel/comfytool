#include "uicafbuildertab.h"
#include "ui_uicafbuildertab.h"

#include "uicafbuilderrootinfo.h"
#include "uicafbuildertablumpinfo.h"

#include <QGridLayout>
#include <QMessageBox>
#include <QPushButton>
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

unsigned currentLump(UIcafbuildertab* cbt) {
    return cbt->buildWidget->currentRow() - 1;
}

void UIcafbuildertab::updateInfoPanel(int current) {
    if(panelWidget != nullptr) {
        findChild<QGridLayout*>("gl_panel")->removeWidget(panelWidget);
        panelWidget->close();
        delete panelWidget;
    }

    if(!current) {
        qInfo()<<"Changing panel to show ROOTINFO";
        UIcafbuilderrootinfo* cbri = new UIcafbuilderrootinfo(findChild<QFrame*>("fr_panel"));
        findChild<QGridLayout*>("gl_panel")->addWidget(cbri, 0, 0, 1, 1, Qt::Alignment());
        cbri->findChild<QSpinBox*> ("sb_cvMajor")->setValue (rootInfo.cvMajor);
        cbri->findChild<QSpinBox*> ("sb_cvMinor")->setValue (rootInfo.cvMinor);
        cbri->findChild<QSpinBox*> ("sb_revision")->setValue(rootInfo.revision);
        cbri->findChild<QLineEdit*>("le_path")->setText     (rootInfo.path);

        connect(cbri->findChild<QPushButton*>("pbtn_apply"), SIGNAL(clicked(bool)), this, SLOT(applyRootSettings()));

        panelWidget = cbri;
        cbri->show();
        return;
    }

    qInfo()<<"Changing panel to show LUMPINFO";
    UIcafbuildertablumpinfo* cbli = new UIcafbuildertablumpinfo(findChild<QFrame*>("fr_panel"));
    findChild<QGridLayout*>("gl_panel")->addWidget(cbli, 0, 0, 1, 1, Qt::Alignment());

    cbli->findChild<QLineEdit*>("le_name")->setText(lumps[currentLump(this)].name);
    cbli->findChild<QLineEdit*>("le_path")->setText(lumps[currentLump(this)].path);
    cbli->findChild<QLineEdit*>("le_type")->setText(lumps[currentLump(this)].type);
    cbli->findChild<QLineEdit*>("le_data")->setText(lumps[currentLump(this)].datapath);

    connect(cbli->findChild<QLineEdit*>("le_name"), SIGNAL(textChanged(QString)), this, SLOT(setUnsaved()));
    connect(cbli->findChild<QLineEdit*>("le_path"), SIGNAL(textChanged(QString)), this, SLOT(setUnsaved()));
    connect(cbli->findChild<QLineEdit*>("le_type"), SIGNAL(textChanged(QString)), this, SLOT(setUnsaved()));
    connect(cbli->findChild<QLineEdit*>("le_data"), SIGNAL(textChanged(QString)), this, SLOT(setUnsaved()));

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

void UIcafbuildertab::addVisItem(QString name, Lump l) {
    lumps.append(l);
    buildWidget->addItem(name);
}

void UIcafbuildertab::setUnsaved() {

}

UIcafbuildertab::~UIcafbuildertab() {
    delete ui;
}
