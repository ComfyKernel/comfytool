#pragma once

#include <QWidget>
#include "ctabscreen.h"
#include "caf.h"

namespace Ui {
class tab_cafviewer;
}

class tab_cafviewer : public CTabScreen {
    Q_OBJECT

public:
    caf asset;

    explicit tab_cafviewer(QWidget *parent = 0);
    ~tab_cafviewer();

    const QList<QMenu*> menus(QWidget *parent) const;
    QString getName();
    bool saveFile(const QString &file) const;
    bool loadFile(const QString &file);
    CTabScreen* makeNew(QWidget *parent) const;

private:
    Ui::tab_cafviewer *ui;
};
