#include "tab_tilesetbuilder.h"
#include "ui_tab_tilesetbuilder.h"

#include <QDebug>

tab_tilesetbuilder::tab_tilesetbuilder(QWidget *parent) :
    CTabScreen(parent),
    ui(new Ui::tab_tilesetbuilder) {
    ui->setupUi(this);

    setLoadable(true);
    setSavable (true);
    setModeChangeAllowed(false);
}

const QList<QMenu*> tab_tilesetbuilder::menus(QWidget *parent) const {
    QList<QMenu*> ret;

    QMenu* m_home = new QMenu(parent);
    m_home->setTitle("Tileset");
    /*QAction* a_about = new QAction(m_home);
    a_about->setText("About");
    m_home->addAction(a_about);*/

    ret.append(m_home);
    return ret;
}

QString tab_tilesetbuilder::getName() {
    return "Tileset";
}

CTabScreen* tab_tilesetbuilder::makeNew(QWidget *parent) const {
    return new tab_tilesetbuilder(parent);
}

bool tab_tilesetbuilder::saveFile(const QString &file) const {
    qInfo()<<"Home Tab does not support file saving";
    return false;
}

bool tab_tilesetbuilder::loadFile(const QString &file) {
    qInfo()<<"Home Tab does not support file loading";
    return false;
}

tab_tilesetbuilder::~tab_tilesetbuilder() {
    delete ui;
}
