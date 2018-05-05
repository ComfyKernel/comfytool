#ifndef TOOLCHOOSER_H
#define TOOLCHOOSER_H

#include <QMainWindow>

namespace Ui {
  class ToolChooser;
}

class ToolChooser : public QMainWindow {
    Q_OBJECT

public:
    explicit ToolChooser(QWidget *parent = 0);
    ~ToolChooser();

private:
    Ui::ToolChooser *ui;
};


#endif // TOOLCHOOSER_H
