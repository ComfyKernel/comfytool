#include "uicafbuildertab.h"
#include "ui_uicafbuildertab.h"

#include "uicafbuilderrootinfo.h"

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
    ucb->buildWidget->addItem("Root");
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
        }
    }

    if(qxsr->hasError()) {
        QMessageBox::critical(this, "Failed parsing CXF", qxsr->errorString(), QMessageBox::Ok);
        return;
    }
}

void UIcafbuildertab::updateInfoPanel(int current) {
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
    }
}

void UIcafbuildertab::applyRootSettings() {
    UIcafbuilderrootinfo* cbri = (UIcafbuilderrootinfo*)panelWidget;
    rootInfo.cvMajor  = cbri->findChild<QSpinBox*>("sb_cvMajor")->value();
    rootInfo.cvMinor  = cbri->findChild<QSpinBox*>("sb_cvMinor")->value();
    rootInfo.revision = cbri->findChild<QSpinBox*>("sb_revision")->value();
    rootInfo.path     = cbri->findChild<QLineEdit*>("le_path")->text();
}

UIcafbuildertab::~UIcafbuildertab() {
    delete ui;
}
