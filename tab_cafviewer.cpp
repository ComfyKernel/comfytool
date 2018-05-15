#include "tab_cafviewer.h"
#include "ui_tab_cafviewer.h"

#include <QAction>
#include <QDebug>
#include <QMenu>

tab_cafviewer::tab_cafviewer(QWidget *parent) :
    CTabScreen(parent),
    ui(new Ui::tab_cafviewer) {
    ui->setupUi(this);
}

const QList<QMenu*> tab_cafviewer::menus(QWidget *parent) const {
    QList<QMenu*> ret;

    /*QMenu* m_home = new QMenu(parent);
    m_home->setTitle("CAF Viewer");
    QAction* a_about = new QAction(m_home);
    a_about->setText("About");
    m_home->addAction(a_about);

    ret.append(m_home);*/
    return ret;
}

QString tab_cafviewer::getName() {
    return "CAF Viewer";
}

CTabScreen* tab_cafviewer::makeNew(QWidget *parent) const {
    return new tab_cafviewer(parent);
}

bool tab_cafviewer::saveFile(const QString &file) const {
    qInfo()<<"CAF Viewer does not support file saving";
    return false;
}

bool tab_cafviewer::loadFile(const QString &file) {
    qInfo()<<"Loading unimplemented";
    return false;
}

tab_cafviewer::~tab_cafviewer() {
    delete ui;
}
