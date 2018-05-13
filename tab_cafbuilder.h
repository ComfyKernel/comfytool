#ifndef TAB_CAFBUILDER_H
#define TAB_CAFBUILDER_H

#include <QTreeWidgetItem>
#include <QVariant>
#include <QWidget>
#include <QList>

#include "ctabscreen.h"

struct Lump : public QObject {
    Q_OBJECT
public:
    Lump(QObject* parent = nullptr);
    Lump(const Lump&, QObject* parent = nullptr);

protected:
    QString _name = "Unnamed Lump";
    QString _path = "/";
    QString _type = "unknown";
    QString _data = "";

    unsigned _revision = 0;

signals:
    void modified();

public slots:
    void setName(const QString& name);
    void setPath(const QString& path);
    void setType(const QString& type);
    void setData(const QString& data);
    void setRevision(unsigned revision);

    void setName(const QVariant& name);
    void setPath(const QVariant& path);
    void setType(const QVariant& type);
    void setData(const QVariant& data);
    void setRevision(const QVariant& revision);

    QString name() const;
    QString path() const;
    QString type() const;
    QString data() const;

    unsigned revision() const;
};

class CT_WI_Helper : public QObject {
    Q_OBJECT
public:
    CT_WI_Helper(QObject* parent = nullptr);

signals:
    void hasChanged(const QVariant&);
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

    QList<Lump*> lumps;

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
    void addRoot           ();

    void setRootPath    (const QVariant&);
    void setRootRevision(const QVariant&);

private:
    Ui::tab_cafbuilder *ui;
};

#endif // TAB_CAFBUILDER_H
