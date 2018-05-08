#ifndef UICAFBUILDERTAB_H
#define UICAFBUILDERTAB_H

#include <QWidget>

namespace Ui {
class UIcafbuildertab;
}

class UIcafbuildertab : public QWidget {
    Q_OBJECT

public:
    QString currentFile;

    explicit UIcafbuildertab(QWidget *parent = 0);
    ~UIcafbuildertab();

public slots:
    void loadFile(QString name="");

private:
    Ui::UIcafbuildertab *ui;
};

#endif // UICAFBUILDERTAB_H
