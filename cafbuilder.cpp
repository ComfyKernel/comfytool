#include "cafbuilder.h"
#include "ui_cafbuilder.h"
#include "quitdialog.h"

#include <QListWidget>
#include <QCloseEvent>
#include <QFileDialog>
#include <QDebug>

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
    QObject::connect(findChild<  QAction*  >("actionOpen"), SIGNAL(triggered(bool)), this, SLOT(openFileDialog()));
}

QString genName(QString name, QString type, QString path, QString data) {
    return (name + " (" + type + ") [" + path + "]" + " {" + data + "}");
}

unsigned int getCurrentIndex(CafBuilder* cfb) {
    return (unsigned int)cfb->findChild<QListWidget*>("list_lumps")->currentIndex().row();
}

void clearBuilder(CafBuilder* cb) {
    cb->lumps.clear();
    cb->findChild<QListWidget*>("list_lumps")->clear();
}

void cafShowMessage(CafBuilder* cb, QString msg, bool log=false) {
    cb->findChild<QStatusBar*>("statusbar")->showMessage(msg);
    if(log) qInfo()<<msg;
}

void pushLump(CafBuilder* cb, QString name, QString type, QString path, QString data, unsigned vMajor=1, unsigned vMinor=0) {
    cb->lumps.append({.name=name, .type=type, .path=path, .data=data, .versionMajor=vMajor, .versionMinor=vMinor});
    cb->findChild<QListWidget*>("list_lumps")->addItem(genName(name, type, path, data));
}

void CafBuilder::loadFile(QString file) {
    cafShowMessage(this, QString("Opening File '" + file + "'"), true);

    QFile qf(file);

    if(!qf.open(QIODevice::ReadOnly | QIODevice::Text)) {
        cafShowMessage(this, QString("Failed to open file '" + file + "'"), true);
        return;
    }

    clearBuilder(this);

    while(!qf.atEnd()) {
        auto data = qf.readLine().split(',');
        pushLump(this, data[0], data[1], data[2], data[3], data[4].simplified().toInt(), data[5].simplified().toInt());
    }

    currentFile=file;
    findChild<QListWidget*>("list_lumps")->setCurrentRow(0);
    showCurrentLump();

    cafShowMessage(this, QString("File '" + file + "' Opened"), true);
}

void CafBuilder::addLumpItem() {
    // Grab text from input fields and add it to a new LUMP item in both the internal list and the visual one

    QString name = findChild<QLineEdit*>("le_name")->text();
    QString path = findChild<QLineEdit*>("le_path")->text();
    QString type = findChild<QLineEdit*>("le_type")->text();
    QString data = findChild<QLineEdit*>("le_data")->text();
    unsigned vMajor = findChild<QSpinBox*>("sb_vmajor")->value();
    unsigned vMinor = findChild<QSpinBox*>("sb_vminor")->value();

    pushLump(this, name, type, path, data, vMajor, vMinor);

    // Disable removing and applying since getCurrent() returns null at this point

    cafShowMessage(this, QString("Added New LUMP '") + name + "'");
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

    cafShowMessage(this, QString("Selected LUMP '") + lumps[current].name + "'");
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

    cafShowMessage(this, QString("Modified LUMP '") + lumps[current].name + "'");
}

void CafBuilder::removeCurrentLump() {
    unsigned int current = getCurrentIndex(this);

    QString name = lumps[current].name;

    lumps.removeAt(current);
    QListWidget* qlw = findChild<QListWidget*>("list_lumps");
    delete qlw->takeItem(current);

    cafShowMessage(this, QString("Removed LUMP '") + name + "'");

    findChild<QPushButton*>("pbtn_apply")->setEnabled(false);
    findChild<QPushButton*>("pbtn_rem")->setEnabled(false);
}

void CafBuilder::openQuitDialog() {
    cafShowMessage(this, QString("Opening Quit Dialog"));

    QuitDialog qd(this);

    QObject::connect(&qd, SIGNAL(accepted()), this, SLOT(close()));

    qd.exec();
}

void CafBuilder::openFileDialog() {
    QString fname = QFileDialog::getOpenFileName(this, tr("Open Comfy Asset Tool File (*.cbf *.caf)"), "", tr("Comfy Asset Tool Files (*.cbf *.caf)"));
    loadFile(fname);
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
