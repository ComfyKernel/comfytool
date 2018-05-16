#pragma once

#include <QWidget>

#include "ctabscreen.h"

namespace Ui {
class tab_tilesetbuilder;
}

class tab_tilesetbuilder : public CTabScreen {
    Q_OBJECT

public:
    explicit tab_tilesetbuilder(QWidget *parent = 0);
    ~tab_tilesetbuilder();

    const QList<QMenu*> menus(QWidget *parent) const;
    QString getName();
    bool saveFile(const QString &file) const;
    bool loadFile(const QString &file);
    CTabScreen* makeNew(QWidget *parent) const;


private:
    Ui::tab_tilesetbuilder *ui;
};
