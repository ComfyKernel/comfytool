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

class ToolMain : public QMainWindow {
    Q_OBJECT
protected:
    QTabWidget* tabbar;
    CTabScreen*    _default_tab;

    typedef QPair<CTabScreen*, QString> _refpair;
    QList<_refpair> _refpairs;

public:
    explicit ToolMain(QWidget *parent = nullptr);
    ~ToolMain();

    void addFileTypeRef(CTabScreen* ref = nullptr, const QString& name = "");
    void removeFileTypeRef(const QString& name = "");

public slots:
    void setDefaultTab  (CTabScreen* tab = nullptr);
    CTabScreen* openTab (CTabScreen* tab = nullptr);
    void openDefaultTab ();
    void openFileDialog ();
    void onTabChanged   (int tab = 0);

private:
    Ui::ToolMain *ui;
};

#endif // TOOLMAIN_H
