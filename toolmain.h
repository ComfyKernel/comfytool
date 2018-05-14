#ifndef TOOLMAIN_H
#define TOOLMAIN_H

#include <QMainWindow>
#include <QTabWidget>
#include <QList>
#include <QPair>

#include "ctabscreen.h"

namespace Ui {
    class ToolMain;
}

typedef QPair<CTabScreen*, QString> _refpair;

class ToolMain : public QMainWindow {
    Q_OBJECT
protected:
    QTabWidget* tabbar;
    CTabScreen* _default_tab;
    QList<_refpair> _refpairs;

public:
    explicit ToolMain(QWidget *parent = nullptr);
    ~ToolMain();

    void addFileTypeRef(CTabScreen* ref = nullptr, const QString& name = "");
    void removeFileTypeRef(const QString& name = "");

public slots:
    void setDefaultTab    (CTabScreen* tab = nullptr);
    CTabScreen* openTab   (CTabScreen* tab = nullptr);
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
