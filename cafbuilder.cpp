#include "cafbuilder.h"
#include "ui_cafbuilder.h"

#include "quitdialog.h"
#include "cafrootsettings.h"

#include <QProgressDialog>
#include <QDesktopWidget>
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

    QObject::connect(findChild<  QAction*  >("actionOpen"   ), SIGNAL(triggered(bool)), this, SLOT(openFileDialog()));
    QObject::connect(findChild<  QAction*  >("actionSave"   ), SIGNAL(triggered(bool)), this, SLOT(saveFile()));
    QObject::connect(findChild<  QAction*  >("actionSave_As"), SIGNAL(triggered(bool)), this, SLOT(openSaveFileDialog()));

    QObject::connect(findChild<  QAction*  >("actionBuild_2"      ), SIGNAL(triggered(bool)), this, SLOT(buildCAF()));
    QObject::connect(findChild<  QAction*  >("actionRoot_Settings"), SIGNAL(triggered(bool)), this, SLOT(openRootSettings()));
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
    cb->findChild<QListWidget*>("list_lumps")->setCurrentRow(cb->lumps.length() - 1);
    cb->showCurrentLump();
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

        if(data[0] == "ROOT") {
            assetPath = data[1];
            avMajor   = data[2].simplified().toInt();
            avMinor   = data[3].simplified().toInt();
            avRev     = data[4].simplified().toInt();

            continue;
        }

        pushLump(this, data[0], data[1], data[2], data[3], data[4].simplified().toInt(), data[5].simplified().toInt());
    }

    qf.close();

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
    QString fname = QFileDialog::getOpenFileName(this, tr("Open Comfy Asset Tool File (*.cbf *.caf)"), "", tr("Comfy Asset Builder File (*.cbf);;Comfy Asset File (*.caf)"));
    loadFile(fname);
}

void CafBuilder::openSaveFileDialog() {
    QString fname = QFileDialog::getSaveFileName(this, tr("Save Comfy Asset Tool File (*.cbf *.caf)"), currentFile, tr("Comfy Asset Builder File (*.cbf);;Comfy Asset File (*.caf)"));
    if(fname.isEmpty()) return;

    currentFile = fname;
    saveFile();
}

void CafBuilder::saveFile() {
    if(currentFile.isEmpty()) {
        openSaveFileDialog();
        return;
    }

    QFile qf(currentFile);

    cafShowMessage(this, QString("Saving File '" + currentFile + "'"), true);

    if(!qf.open(QIODevice::WriteOnly | QIODevice::Text)) {
        cafShowMessage(this, QString("Failed to open file '" + currentFile + "' for saving"), true);
        return;
    }

    std::string rstr = QString(QString("ROOT,") + assetPath + "," + QString::number(avMajor) + "," + QString::number(avMinor) + "," + QString::number(avRev) + "\n").toStdString();
    qf.write((const char*)rstr.c_str());

    for(const auto& l : lumps) {
        std::string str = QString(l.name + "," + l.type + "," + l.path + "," + l.data + "," + QString::number(l.versionMajor) + "," + QString::number(l.versionMinor) + "\n").toStdString();

        qf.write((const char*)str.c_str());
    }

    qf.close();

    cafShowMessage(this, QString("File '" + currentFile + "' Saved!"), true);
}

void CafBuilder::openRootSettings() {
    cafShowMessage(this, "Editing Root Settings");

    CafRootSettings crs(this);
    diaREF = &crs;

    crs.findChild<QLineEdit*>("le_rootname")->setText(assetPath);
    crs.findChild<QSpinBox*>("sb_vmajor")->setValue(avMajor);
    crs.findChild<QSpinBox*>("sb_vminor")->setValue(avMinor);
    crs.findChild<QSpinBox*>("sb_vrev"  )->setValue(avRev  );

    QObject::connect(crs.findChild<QPushButton*>("pbtn_apply"), SIGNAL(clicked(bool)), this, SLOT(applyRootChanges()));

    if(crs.exec() == QDialog::Accepted) {
        applyRootChanges();
        return;
    }

    cafShowMessage(this, "Root Settings Change Cancelled");
}

void CafBuilder::applyRootChanges() {
    CafRootSettings* crs = (CafRootSettings*)diaREF;

    assetPath = crs->findChild<QLineEdit*>("le_rootname")->text();

    avMajor = crs->findChild<QSpinBox*>("sb_vmajor")->value();
    avMinor = crs->findChild<QSpinBox*>("sb_vminor")->value();
    avRev   = crs->findChild<QSpinBox*>("sb_vrev"  )->value();

    cafShowMessage(this, "Changed Root Settings");
}

void CafBuilder::buildCAF() {
    cafShowMessage(this, "Building CAF File");

    QString fname = QFileDialog::getSaveFileName(this, tr("Build CAF File (*.caf)"), currentFile.split('.')[0], tr("Comfy Asset File (*.caf)"));
    if(fname.isEmpty()) return;

    QFile qf(fname);

    if(!qf.open(QIODevice::WriteOnly)) {
        cafShowMessage(this, QString("Failed to open file '" + currentFile + "' for building"), true);
        return;
    }

    // Write HEAD //

    unsigned int c_off;

    qf.write("CAF");

    uint16_t ct_us = avMajor;
    qf.write((const char*)&ct_us, 2);                    // CAF Major version
    ct_us = avMinor;
    qf.write((const char*)&ct_us, 2);                    // CAF Minor version

    uint32_t ct_ui = avRev;
    qf.write((const char*)&ct_ui, 4);                    // Asset Revision
    ct_ui = 15 + assetPath.length() + 1;
    qf.write((const char*)&ct_ui, 4);                    // Pointer to first LUMPITEM

    std::string ct_string = assetPath.toStdString();
    qf.write(ct_string.c_str(), ct_string.length() + 1); // Asset Path

    c_off = 15 + assetPath.length() + 1;

    ////////////////

    // Write LUMPS //

    for(unsigned int l=0; l<lumps.length(); ++l) {
        const Lump& i = lumps[l];

        QString fpath = fname;
        fpath.chop(fpath.length() - fpath.lastIndexOf('/') - 1);
        fpath += i.data;

        cafShowMessage(this, QString("Building LUMP '") + i.name + "' With File '" + fpath + "'", true);

        QFile qfl(fpath);
        if(!qfl.open(QIODevice::ReadOnly)) {
            cafShowMessage(this, QString("Failed to open file '" + fpath + "' used in LUMP '" + i.name + "'"), true);
            qf.close();
            return;
        }

        // Write Flags !!STUB!!
        if(l != lumps.length() - 1) {
            ct_us = 0b1000000000000000;
        } else {
            ct_us = 0;
        }
        qf.write((const char*)&ct_us, 2);                         // Flag A
        ct_us = 0;
        qf.write((const char*)&ct_us, 2);                         // Flag B

        // Write Attributes
        unsigned int frd_size = 18 + i.name.length() + i.path.length() + i.type.length();
        unsigned int prd_size = qfl.size() + frd_size;
        ct_ui = prd_size;
        qf.write((const char*)&ct_ui, 4);                         // LUMP Size
        ct_ui = c_off + frd_size;
        qf.write((const char*)&ct_ui, 4);                         // LUMP Pointer

        ct_string = i.name.toStdString();
        qf.write(ct_string.c_str(), ct_string.length() + 1);      // LUMP Name
        ct_string = i.path.toStdString();
        qf.write(ct_string.c_str(), ct_string.length() + 1);      // LUMP Path
        ct_string = i.type.toStdString();
        qf.write(ct_string.c_str(), ct_string.length() + 1);      // LUMP Type

        c_off += prd_size;

        qf.write(qfl.readAll());

        qfl.close();
    }

    /////////////////

    qf.close();

    cafShowMessage(this, "Finished Building CAF");
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
