#ifndef CAFROOTSETTINGS_H
#define CAFROOTSETTINGS_H

#include <QWidget>
#include <QDialog>

namespace Ui {
  class CafRootSettings;
}

class CafRootSettings : public QDialog {
    Q_OBJECT

public:
    explicit CafRootSettings(QWidget *parent = 0);
    ~CafRootSettings();

private:
    Ui::CafRootSettings *ui;
};

#endif // CAFROOTSETTINGS_H
