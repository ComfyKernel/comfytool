#include "mainwindow.h"
#include "toolchooser.h"
#include "cafbuilder.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <QDebug>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    /*w.show();

    QVBoxLayout* qvb = (QVBoxLayout*)w.findChild<QVBoxLayout*>("verticalLayout_3");
    qvb->addStretch(1);
    qInfo()<<qvb;

    return a.exec();*/

    CafBuilder  cb;
    ToolChooser tc;

    QPushButton* pbtn_mapedit = tc.findChild<QPushButton*>("pbtn_mapedit");
    QObject::connect(pbtn_mapedit, SIGNAL(clicked(bool)), &w, SLOT(show()));

    QPushButton* pbtn_cafbuild = tc.findChild<QPushButton*>("pbtn_cafbuild");
    QObject::connect(pbtn_cafbuild, SIGNAL(clicked(bool)), &cb, SLOT(show()));

    tc.show();

    return a.exec();
}
