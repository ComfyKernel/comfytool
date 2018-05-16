#include <QApplication>

#include "ctabscreen.h"

#include "tab_home.h"
#include "tab_cafbuilder.h"
#include "tab_cafviewer.h"
#include "tab_tilesetbuilder.h"

#include "toolmain.h"

int main(int argc, char *argv[]) {
    QApplication qapp(argc, argv);

    ToolMain* tm = new ToolMain;
    tm->show();

    tab_home* htab = new tab_home(tm);
    tm->setDefaultTab(htab);
    tm->openDefaultTab();

    tm->addFileTypeRef(new tab_cafbuilder    (tm), "cafbuilder");
    tm->addFileTypeRef(new tab_tilesetbuilder(tm), "tileset");
    tm->addFileTypeRef(new tab_cafviewer     (tm), ".caf");

    return qapp.exec();
}
