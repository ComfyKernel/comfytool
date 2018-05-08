#ifndef UIHOMETAB_H
#define UIHOMETAB_H

#include <QWidget>

namespace Ui {
class UIhometab;
}

class UIhometab : public QWidget
{
    Q_OBJECT

public:
    explicit UIhometab(QWidget *parent = 0);
    ~UIhometab();

private:
    Ui::UIhometab *ui;
};

#endif // UIHOMETAB_H
