#ifndef UICAFBUILDERROOTINFO_H
#define UICAFBUILDERROOTINFO_H

#include <QWidget>

namespace Ui {
class UIcafbuilderrootinfo;
}

class UIcafbuilderrootinfo : public QWidget
{
    Q_OBJECT

public:
    explicit UIcafbuilderrootinfo(QWidget *parent = 0);
    ~UIcafbuilderrootinfo();

private:
    Ui::UIcafbuilderrootinfo *ui;
};

#endif // UICAFBUILDERROOTINFO_H
