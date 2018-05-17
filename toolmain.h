#ifndef TOOLMAIN_H
#define TOOLMAIN_H

#include <QMainWindow>
#include <QTabWidget>
#include <QList>
#include <QPair>

#include "ctabscreen.h"

class ToolMain;

namespace Ui {
    class ToolMain;
}

typedef CTabScreen* (*_cfunc)(ToolMain*);
typedef QPair<_cfunc, QString> _refpair;

class ToolMain : public QMainWindow {
    Q_OBJECT
protected:
    QTabWidget*     tabbar;
    _cfunc          _default_tab;
    QList<_refpair> _refpairs;

public:
    explicit ToolMain(QWidget *parent = nullptr);
    ~ToolMain();

    void addFileTypeRef   (_cfunc ref = nullptr, const QString& name = "");
    void removeFileTypeRef(const QString& name = "");

public slots:
    void setDefaultTab    (_cfunc tab = nullptr);
    CTabScreen* openTab   (_cfunc tab = nullptr);
    void openDefaultTab   ();
    void openFileDialog   ();
    void openFile         (QString fname = "");
    void openSaveDialog   ();
    void saveFile         ();
    void onTabChanged     (int tab = 0);
    void closeTab         (int tab = 0);
    CTabScreen* currentTab();

private:
    Ui::ToolMain *ui;
};

#endif // TOOLMAIN_H
