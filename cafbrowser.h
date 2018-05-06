#ifndef CAFBROWSER_H
#define CAFBROWSER_H

#include "lump.h"

#include <QMainWindow>
#include <QList>

namespace Ui {
class CafBrowser;
}

class CafBrowser : public QMainWindow {
    Q_OBJECT

public:
    QList<Lump> lumps;
    QString assetPath;

    explicit CafBrowser(QWidget *parent = 0);
    ~CafBrowser();
    void loadFile(QString file="");

public slots:
    void openFileDialog();

private:
    Ui::CafBrowser *ui;
};

#endif // CAFBROWSER_H
