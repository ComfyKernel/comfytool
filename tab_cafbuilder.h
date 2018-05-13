#ifndef TAB_CAFBUILDER_H
#define TAB_CAFBUILDER_H

#include <QTreeWidgetItem>
#include <QWidget>
#include <QList>

#include "ctabscreen.h"

struct Lump {
public:
    QString name = "Unnamed Lump";
    QString path = "/";
    QString type = "unknown";
    QString data = "";

    unsigned revision = 0;
};

namespace Ui {
    class tab_cafbuilder;
}

class tab_cafbuilder : public CTabScreen {
    Q_OBJECT
protected:
    struct {
        QString  path = "/unknown/";
        unsigned revision = 0;
    } rootinfo;

    QList<Lump> lumps;

public:
    explicit tab_cafbuilder(QWidget *parent = 0);
    ~tab_cafbuilder();

    const QList<QMenu*> menus(QWidget *parent) const;
    QString getName();
    bool saveFile(const QString &file) const;
    bool loadFile(const QString &file);
    CTabScreen* makeNew(QWidget *parent) const;

public slots:
    void handleValueChanged(QTreeWidgetItem* item, int column);
    void addLump           (const Lump& lump);

private:
    Ui::tab_cafbuilder *ui;
};

#endif // TAB_CAFBUILDER_H
