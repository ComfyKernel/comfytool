#ifndef CAFBUILDER_H
#define CAFBUILDER_H

#include "lump.h"
#include <QMainWindow>

namespace Ui {
  class CafBuilder;
}

class CafBuilder : public QMainWindow {
    Q_OBJECT

public:
    QList<Lump> lumps;
    QString currentFile;

    QString assetPath;
    unsigned avMajor, avMinor, avRev;

    void* diaREF;

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
    void openSaveFileDialog();
    void saveFile();

    void openRootSettings();
    void applyRootChanges();

    void buildCAF();

    void closeEvent(QCloseEvent *event);

private:
    Ui::CafBuilder *ui;
};

#endif // CAFBUILDER_H
