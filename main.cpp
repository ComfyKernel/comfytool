#include "toolchooser.h"

#include <QApplication>
#include <QWidget>
#include <QDebug>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    ToolChooser tc;

    tc.show();

    return a.exec();
}
