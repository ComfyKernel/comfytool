#include <QApplication>

#include "uicomfytool.h"

int main(int argc, char *argv[]) {
    QApplication qapp(argc, argv);

    UIcomfytool uct;
    uct.show();

    return qapp.exec();
}
