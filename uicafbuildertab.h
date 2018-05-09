#ifndef UICAFBUILDERTAB_H
#define UICAFBUILDERTAB_H

#include <QXmlStreamReader>
#include <QListWidget>
#include <QWidget>

#include "lump.h"

namespace Ui {
class UIcafbuildertab;
}

class UIcafbuildertab : public QWidget {
    Q_OBJECT

public:
    QString currentFile;

    QListWidget* buildWidget;
    QWidget*     panelWidget;

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
    void loadFile(QString name="");
    void parseCXF(QString name="", QXmlStreamReader* qxsr=nullptr);

    void updateInfoPanel(int current=0);
    void applyRootSettings();

    void addVisItem(QString name="New Item", Lump lump=Lump());
    void setUnsaved();

private:
    Ui::UIcafbuildertab *ui;
};

#endif // UICAFBUILDERTAB_H
