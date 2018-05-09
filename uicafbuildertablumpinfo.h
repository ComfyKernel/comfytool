#ifndef UICAFBUILDERTABLUMPINFO_H
#define UICAFBUILDERTABLUMPINFO_H

#include <QWidget>

namespace Ui {
class UIcafbuildertablumpinfo;
}

class UIcafbuildertablumpinfo : public QWidget
{
    Q_OBJECT

public:
    explicit UIcafbuildertablumpinfo(QWidget *parent = 0);
    ~UIcafbuildertablumpinfo();

private:
    Ui::UIcafbuildertablumpinfo *ui;
};

#endif // UICAFBUILDERTABLUMPINFO_H
