#ifndef CAFBUILDER_H
#define CAFBUILDER_H

#include <QMainWindow>

namespace Ui {
  class CafBuilder;
}

struct Lump {
public:
    QString name;
    QString type;
    QString path;
    QString data;

    unsigned int versionMajor;
    unsigned int versionMinor;
};

class CafBuilder : public QMainWindow {
    Q_OBJECT

public:
    QList<Lump> lumps;
    QString currentFile;

    explicit CafBuilder(QWidget *parent = 0);
    ~CafBuilder();

    void loadFile(QString file = "");

public slots:
    void addLumpItem();
    void showCurrentLump();
    void applyLumpChange();
    void removeCurrentLump();
    void openQuitDialog();
    void openFileDialog();

    void closeEvent(QCloseEvent *event);

private:
    Ui::CafBuilder *ui;
};

#endif // CAFBUILDER_H
