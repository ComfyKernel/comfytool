#include "cafbrowser.h"
#include "ui_cafbrowser.h"

#include <QFileDialog>
#include <QDebug>
#include <QFile>

CafBrowser::CafBrowser(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CafBrowser) {
    ui->setupUi(this);
    QObject::connect(findChild<QAction*>("actionOpen"), SIGNAL(triggered(bool)), this, SLOT(openFileDialog()));
}

void cafShowMessage(CafBrowser* cb, QString msg, bool log=false) {
    cb->findChild<QStatusBar*>("statusbar")->showMessage(msg);
    if(log) qInfo()<<msg;
}

QString getHex(unsigned num) {
    return QString("0x") + QString("%1").arg(num, 1, 16).toUpper();
}

QString readString(QFile& fi, uint32_t& off) {
    QString str;
    uint32_t voff;

    char c;
    while(fi.getChar(&c)) {
        if(!c) {
            off = voff;
            return str;
        }

        str += c;
        voff++;
    }

    off = voff;
    return str;
}

void CafBrowser::loadFile(QString file) {
    cafShowMessage(this, QString("Loading file '") + file + "'", true);

    QFile qf(file);

    if(!qf.open(QIODevice::ReadOnly)) {
        cafShowMessage(this, QString("Failed to load file '" + file + "'"), true);
        return;
    }

    QByteArray qdata = qf.readAll();
    char* data = qdata.data();

    cafShowMessage(this, QString("File Size : ") + QString::number(qdata.length()));

    if((data[0] == 0x43) && (data[1] == 0x41) && (data[2] == 0x46)) {
        cafShowMessage(this, QString("File header validated"), true);
    } else {
        cafShowMessage(this, QString("Failed to validate CAF header"), true);
        return;
    }

    struct {
        uint16_t vMajor;
        uint16_t vMinor;
        uint32_t vRev;
        uint32_t lPtr;
    } hinfo;

    memcpy(&hinfo, data + 3, 12);

    uint32_t s_off;
    QString apname = readString(qf, s_off);

    cafShowMessage(this, QString("Header info [") + data[0] + data[1] + data[2] + "] |"
                                 + "CAF Version v" + QString::number(hinfo.vMajor) + "." + QString::number(hinfo.vMinor)
                                 + " | Asset Revision " + QString::number(hinfo.vRev) + " | LUMP Item Pointer " + getHex(hinfo.lPtr)
                                 + " | Path Name " + apname, true);

    findChild<QLabel*>("lbl_root_name")->setText(QString("Asset Root : <em>") + apname + "</em>");
    assetPath = apname;

    cafShowMessage(this, QString("Reading LUMPS"), true);

    uint32_t cl_ptr = hinfo.lPtr;
    // uint32_t cl_ptr = 0;
    bool itemExists = true;
    while(itemExists) {
        struct {
            uint16_t flags;
            uint32_t lump_size;
            uint32_t ptr_lump;
            uint16_t vMajor;
            uint16_t vMinor;
            uint32_t ptr_lumpitem;
        } linfo;
        memcpy(&linfo, data + cl_ptr, 16);
        cl_ptr += 16;

        itemExists = false;

        cafShowMessage(this, QString("LUMP Data | Flags ") + getHex(linfo.flags) + " | LUMP Data Pointer " + getHex(linfo.ptr_lump) +
                                     " | Version " + QString::number(linfo.vMajor) + "." + QString::number(linfo.vMinor) + " | LUMP Item Pointer "
                                     + getHex(linfo.ptr_lumpitem) + " | Lump Size " + getHex(linfo.lump_size), true);
    }

    qf.close();

    cafShowMessage(this, QString("Loaded file '") + file + "'", true);
}

void CafBrowser::openFileDialog() {
    QString fname = QFileDialog::getOpenFileName(this, tr("Open Comfy Asset File (*.caf)"), "", tr("Comfy Asset File (*.caf)"));
    loadFile(fname);
}

CafBrowser::~CafBrowser()
{
    delete ui;
}
