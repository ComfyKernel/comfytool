#include "cafbuilder.h"
#include "ui_cafbuilder.h"
#include "quitdialog.h"

#include <QListWidget>
#include <QCloseEvent>

CafBuilder::CafBuilder(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CafBuilder) {
    ui->setupUi(this);

    // Connect base signals

    QListWidget* qlv = findChild<QListWidget*>("list_lumps");
    QObject::connect(qlv, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(showCurrentLump()));

    QObject::connect(findChild<QPushButton*>("pbtn_add"  ), SIGNAL(clicked(bool)  ), this, SLOT(addLumpItem()));
    QObject::connect(findChild<QPushButton*>("pbtn_apply"), SIGNAL(clicked(bool)  ), this, SLOT(applyLumpChange()));
    QObject::connect(findChild<QPushButton*>("pbtn_rem"  ), SIGNAL(clicked(bool)  ), this, SLOT(removeCurrentLump()));
    QObject::connect(findChild<  QAction*  >("actionExit"), SIGNAL(triggered(bool)), this, SLOT(openQuitDialog()));
}

QString genName(QString name, QString type, QString path, QString data) {
    return (name + " (" + type + ") [" + path + "]" + " {" + data + "}");
}

unsigned int getCurrentIndex(CafBuilder* cfb) {
    return (unsigned int)cfb->findChild<QListWidget*>("list_lumps")->currentIndex().row();
}

void CafBuilder::addLumpItem() {
    // Grab text from input fields and add it to a new LUMP item in both the internal list and the visual one

    QString name = findChild<QLineEdit*>("le_name")->text();
    QString path = findChild<QLineEdit*>("le_path")->text();
    QString type = findChild<QLineEdit*>("le_type")->text();
    QString data = findChild<QLineEdit*>("le_data")->text();
    unsigned vMajor = findChild<QSpinBox*>("sb_vmajor")->value();
    unsigned vMinor = findChild<QSpinBox*>("sb_vminor")->value();

    QListWidget* qlv = findChild<QListWidget*>("list_lumps");
    qlv->addItem(genName(name, type, path, data));
    lumps.append({.name=name, .type=type, .path=path, .data=data, .versionMajor=vMajor, .versionMinor=vMinor });

    // Disable removing and applying since getCurrent() returns null at this point

    findChild<QStatusBar*>("statusbar")->showMessage(QString("Added New LUMP \"") + name + "\"");
    findChild<QPushButton*>("pbtn_apply")->setEnabled(false);
    findChild<QPushButton*>("pbtn_rem")->setEnabled(false);
}

void CafBuilder::showCurrentLump() {
    unsigned int current = getCurrentIndex(this);

    findChild<QLineEdit*>("le_name")->setText(lumps[current].name);
    findChild<QLineEdit*>("le_path")->setText(lumps[current].path);
    findChild<QLineEdit*>("le_type")->setText(lumps[current].type);
    findChild<QLineEdit*>("le_data")->setText(lumps[current].data);
    findChild<QSpinBox*>("sb_vmajor")->setValue(lumps[current].versionMajor);
    findChild<QSpinBox*>("sb_vminor")->setValue(lumps[current].versionMinor);

    findChild<QStatusBar*>("statusbar")->showMessage(QString("Selected LUMP \"") + lumps[current].name + "\"");
    findChild<QPushButton*>("pbtn_apply")->setEnabled(true);
    findChild<QPushButton*>("pbtn_rem")->setEnabled(true);
}

void CafBuilder::applyLumpChange() {
    unsigned int current = getCurrentIndex(this);

    lumps[current].name = findChild<QLineEdit*>("le_name")->text();
    lumps[current].path = findChild<QLineEdit*>("le_path")->text();
    lumps[current].type = findChild<QLineEdit*>("le_type")->text();
    lumps[current].data = findChild<QLineEdit*>("le_data")->text();
    lumps[current].versionMajor = findChild<QSpinBox*>("sb_vmajor")->value();
    lumps[current].versionMinor = findChild<QSpinBox*>("sb_vminor")->value();

    findChild<QListWidget*>("list_lumps")->item(current)->setText(genName(lumps[current].name, lumps[current].type, lumps[current].path, lumps[current].data));

    findChild<QStatusBar*>("statusbar")->showMessage(QString("Modified LUMP \"") + lumps[current].name + "\"");
}

void CafBuilder::removeCurrentLump() {
    unsigned int current = getCurrentIndex(this);

    QString name = lumps[current].name;

    lumps.removeAt(current);
    QListWidget* qlw = findChild<QListWidget*>("list_lumps");
    delete qlw->takeItem(current);

    findChild<QStatusBar*>("statusbar")->showMessage(QString("Removed LUMP \"") + name + "\"");

    findChild<QPushButton*>("pbtn_apply")->setEnabled(false);
    findChild<QPushButton*>("pbtn_rem")->setEnabled(false);
}

void CafBuilder::openQuitDialog() {
    findChild<QStatusBar*>("statusbar")->showMessage(QString("Opening Quit Dialog"));

    QuitDialog qd(this);

    QObject::connect(&qd, SIGNAL(accepted()), this, SLOT(close()));

    qd.exec();
}

void CafBuilder::closeEvent(QCloseEvent *event) {
    QuitDialog qd(this);

    int dcode = qd.exec();

    if(dcode == QDialog::Accepted) {
        event->accept();
        return;
    }

    event->ignore();
}

CafBuilder::~CafBuilder() {
    delete ui;
}
