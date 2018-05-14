#include "tab_home.h"
#include "ui_tab_home.h"

#include "toolmain.h"

#include <QListWidgetItem>
#include <QPushButton>
#include <QVBoxLayout>
#include <QListWidget>
#include <QSettings>
#include <QDebug>

tab_home::tab_home(QWidget *parent) :
    CTabScreen(parent),
    ui(new Ui::tab_home) {
    ui->setupUi(this);

    findChild<QVBoxLayout*>("vl_main")->addStretch();

    if(QString(parent->metaObject()->className()) == "ToolMain") {
        ToolMain* tm = (ToolMain*)parent;
        connect(findChild<QPushButton*>("pbtn_openfile"), &QPushButton::clicked,
                tm->findChild<QAction*>("actionOpen"), &QAction::trigger);
    }

    setSavable (false);
    setLoadable(false);
    setModeChangeAllowed(false);

    QListWidget* qlw = findChild<QListWidget*>("lw_recent");

    connect(qlw, &QListWidget::itemDoubleClicked,
            [&](QListWidgetItem* item) {
                ToolMain* tm = (ToolMain*)parent;
                tm->openFile(item->text());
            });

    QSettings settings;
    QList<QString> recents = settings.value("io/recentfiles").value<QList<QString>>();
    std::reverse(recents.begin(), recents.end());
    for(const auto& i : recents) {
        QListWidgetItem* qlwi = new QListWidgetItem(qlw);
        qlwi->setText(i);
    }
}

const QList<QMenu*> tab_home::menus(QWidget *parent) const {
    QList<QMenu*> ret;

    QMenu* m_home = new QMenu(parent);
    m_home->setTitle("Home");
    QAction* a_about = new QAction(m_home);
    a_about->setText("About");
    m_home->addAction(a_about);

    ret.append(m_home);
    return ret;
}

QString tab_home::getName() {
    return "Home";
}

CTabScreen* tab_home::makeNew(QWidget *parent) const {
    return new tab_home(parent);
}

bool tab_home::saveFile(const QString &file) const {
    qInfo()<<"Home Tab does not support file saving";
    return false;
}

bool tab_home::loadFile(const QString &file) {
    qInfo()<<"Home Tab does not support file loading";
    return false;
}

tab_home::~tab_home() {
    delete ui;
}
