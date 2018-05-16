#include "tab_cafviewer.h"
#include "ui_tab_cafviewer.h"

#include <QVBoxLayout>
#include <QListWidget>
#include <QAction>
#include <QLabel>
#include <QDebug>
#include <QFrame>
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
    asset.load(file.toStdString());

    QVBoxLayout* vl = findChild<QVBoxLayout*>("vl_data");
    vl->addStretch();
    vl->setDirection(QVBoxLayout::BottomToTop);

    for(unsigned i = asset.lumps.size(); i > 0; --i) {
        const lump& l = asset.lumps[i - 1];
        QFrame* qf_main = new QFrame(vl->widget());
        qf_main->setFrameShape(QFrame::StyledPanel);
        qf_main->setFrameShadow(QFrame::Sunken);

        QVBoxLayout* qvbl = new QVBoxLayout();
        qvbl->setSpacing(0);

        QLabel* ql = new QLabel(qf_main);
        ql->setText(QString((l.s_path + l.s_name).c_str()));
        ql->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
        qvbl->addWidget(ql);

        ql = new QLabel(qf_main);
        ql->setText(QString(l.s_type.c_str()));
        ql->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
        qvbl->addWidget(ql);

        qvbl->addStretch();
        qvbl->setSizeConstraint(QLayout::SetFixedSize);

        qf_main->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
        qf_main->setLayout(qvbl);

        vl->addWidget(qf_main);
    }

    return false;
}

tab_cafviewer::~tab_cafviewer() {
    delete ui;
}