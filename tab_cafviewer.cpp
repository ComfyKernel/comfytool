#include "tab_cafviewer.h"
#include "ui_tab_cafviewer.h"

#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QAction>
#include <QLabel>
#include <QDebug>
#include <QFrame>
#include <QMenu>

tab_cafviewer::tab_cafviewer(QWidget *parent) :
    CTabScreen(parent),
    ui(new Ui::tab_cafviewer) {
    ui->setupUi(this);

    QScrollArea* qsa = findChild<QScrollArea*>("sa_main");
    QWidget* qw = new QWidget(qsa);
    QVBoxLayout* qvbl = new QVBoxLayout(qw);
    qvbl->setObjectName("vl_data");
    qvbl->addStretch();
    qvbl->setDirection(QVBoxLayout::BottomToTop);
    qw->setLayout(qvbl);

    qsa->setWidget(qw);
    qsa->setWidgetResizable(true);
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

    QFont qf_mono("Monospace");
    qf_mono.setStyleHint(QFont::TypeWriter);

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

        QVBoxLayout* vbl = new QVBoxLayout();
        QFrame* qif = new QFrame(qf_main);
        qif->setFrameShape(QFrame::StyledPanel);
        qif->setFrameShadow(QFrame::Sunken);
        qif->setContentsMargins(0, 0, 0, 0);
        qif->setLayout(vbl);

        QPushButton* qpb = new QPushButton(qif);
        qpb->setText("View");
        qpb->setCheckable(true);
        qpb->setChecked(false);
        vbl->addWidget(qpb);

        qvbl->addWidget(qif);

        if(QString(l.s_type.c_str()).contains("text/")) {
            ql = new QLabel(qif);
            ql->setFrameShape(QFrame::StyledPanel);
            ql->setFrameShadow(QFrame::Raised);
            ql->setObjectName("l_data");
            QString _str;
            for(unsigned i = 0; i < l.s_lump_size; ++i) {
                _str += l.c_lump_data[i];
            }
            ql->setFont(qf_mono);

            ql->setText(_str);
            ql->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
            ql->hide();
            vbl->addWidget(ql);

            connect(qpb, &QPushButton::toggled, [=](bool checked) {
                if(!checked) {
                    ql->hide();
                } else {
                    ql->show();
                }
            });
        }

        if(QString(l.s_type.c_str()).contains("image/png")) {
            ql = new QLabel(qif);
            ql->setFrameShape(QFrame::StyledPanel);
            ql->setFrameShadow(QFrame::Raised);
            ql->setObjectName("l_data");
            QPixmap qmp;
            qmp.loadFromData((uchar*)l.c_lump_data, l.s_lump_size);
            ql->setPixmap(qmp);

            ql->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
            ql->hide();
            vbl->addWidget(ql);

            connect(qpb, &QPushButton::toggled, [=](bool checked) {
                if(!checked) {
                    ql->hide();
                } else {
                    ql->show();
                }
            });
        }

        qvbl->addStretch();
        qvbl->setSizeConstraint(QLayout::SetMinimumSize);

        qf_main->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
        qf_main->setLayout(qvbl);

        vl->addWidget(qf_main);
    }

    return false;
}

tab_cafviewer::~tab_cafviewer() {
    delete ui;
}
