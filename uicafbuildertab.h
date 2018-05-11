#ifndef UICAFBUILDERTAB_H
#define UICAFBUILDERTAB_H

#include <QXmlStreamReader>
#include <QListWidget>
#include <QWidget>

#include "uicomfytool.h"
#include "lump.h"

namespace Ui {
class UIcafbuildertab;
}

class UIcafbuildertab : public QWidget {
    Q_OBJECT

public:
    bool unsaved = false;
    unsigned changedItems = 0;
    unsigned currLump = 0;

    bool    saveset = false;
    QString currentFile = "";

    QListWidget* buildWidget = nullptr;
    QWidget*     panelWidget = nullptr;

    UIcomfytool* _ct = nullptr;

    struct {
        unsigned cvMajor = 1;
        unsigned cvMinor = 1;
        unsigned revision = 0;
        QString path;
    } rootInfo;

    QList<Lump> lumps;

    explicit UIcafbuildertab(QWidget *parent = 0);
    ~UIcafbuildertab();

public slots:
    void saveFile(QString name="");
    void loadFile(QString name="");
    void parseCXF(QString name="", QXmlStreamReader* qxsr=nullptr);

    void updateInfoPanel(int current=0);
    void applyRootSettings();
    void applyLumpSettings(int lump=0);
    void resetRootSettings();
    void resetLumpSettings();
    void openSaveDialog(bool skipdialog);

    void addVisItem(QString name="New Item", Lump lump=Lump());
    void setUnsaved();

private:
    Ui::UIcafbuildertab *ui;
};

#endif // UICAFBUILDERTAB_H
