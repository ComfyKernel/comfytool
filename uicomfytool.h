#ifndef UICOMFYTOOL_H
#define UICOMFYTOOL_H

#include <QMainWindow>
#include <QCloseEvent>

namespace Ui {
class UIcomfytool;
}

class UIcomfytool : public QMainWindow
{
    Q_OBJECT

public:
    QTabWidget* mainTabBar;

    explicit UIcomfytool(QWidget *parent = 0);
    ~UIcomfytool();

public slots:
    void closeEvent(QCloseEvent *event);

    void addMainbarTab   (QString name="New Tab", QWidget* qw=nullptr);
    void removeMainbarTab(int tab=0);
    void openHomeTab();
    void openCAFBuilder(QString name="");

    void openFileDialog  ();

private:
    Ui::UIcomfytool *ui;
};

#endif // UICOMFYTOOL_H
