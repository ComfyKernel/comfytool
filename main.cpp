#include <QApplication>

#include "ctabscreen.h"
#include "tab_home.h"
#include "toolmain.h"

int main(int argc, char *argv[]) {
    QApplication qapp(argc, argv);

    ToolMain* tm = new ToolMain;
    tm->show();

    tab_home* htab = new tab_home(tm);
    tm->setDefaultTab(htab);
    tm->openDefaultTab();

    return qapp.exec();
}
