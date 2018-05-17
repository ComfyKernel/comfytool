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

    tm->setDefaultTab([](ToolMain* t)->CTabScreen*{return new tab_home(t);});

    tm->addFileTypeRef([](ToolMain* t)->CTabScreen*{return new tab_cafbuilder    (t);}, "cafbuilder");
    tm->addFileTypeRef([](ToolMain* t)->CTabScreen*{return new tab_tilesetbuilder(t);}, "tileset"   );
    tm->addFileTypeRef([](ToolMain* t)->CTabScreen*{return new tab_cafviewer     (t);}, ".caf"      );

    tm->openDefaultTab();

    return qapp.exec();
}
