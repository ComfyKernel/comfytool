#ifndef TOOLMAIN_H
#define TOOLMAIN_H

#include <QMainWindow>
#include <QTabWidget>

#include "ctabscreen.h"

namespace Ui {
    class ToolMain;
}

class ToolMain : public QMainWindow {
    Q_OBJECT
protected:
    QTabWidget* tabbar;
    CTabScreen*    _default_tab;

public:
    explicit ToolMain(QWidget *parent = 0);
    ~ToolMain();

public slots:
    void setDefaultTab  (CTabScreen* tab);
    void openTab        (CTabScreen* tab);
    void openDefaultTab ();
    void openFileDialog ();

private:
    Ui::ToolMain *ui;
};

#endif // TOOLMAIN_H
