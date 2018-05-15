#pragma once

#include <QListWidgetItem>
#include <QWidget>
#include "ctabscreen.h"

namespace Ui {
class tab_home;
}

class tab_home : public CTabScreen {
    Q_OBJECT

public:
    explicit tab_home(QWidget *parent = 0);
    ~tab_home();

    const QList<QMenu*> menus(QWidget *parent) const;
    QString getName();
    bool saveFile(const QString &file) const;
    bool loadFile(const QString &file);
    CTabScreen* makeNew(QWidget *parent) const;

private:
    Ui::tab_home *ui;
};
