#ifndef UICAFBUILDERTAB_H
#define UICAFBUILDERTAB_H

#include <QXmlStreamReader>
#include <QListWidget>
#include <QWidget>

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

    explicit UIcafbuildertab(QWidget *parent = 0);
    ~UIcafbuildertab();

public slots:
    void loadFile(QString name="");
    void parseCXF(QString name="", QXmlStreamReader* qxsr=nullptr);

    void updateInfoPanel(int current=0);

    void applyRootSettings();

private:
    Ui::UIcafbuildertab *ui;
};

#endif // UICAFBUILDERTAB_H
