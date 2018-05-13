#pragma once

#include <QWidget>
#include <QMenu>
#include <QList>

class CTabScreen : public QWidget {
    Q_OBJECT
protected:
    QString _current_file = "";
    bool    _is_savable  = false;
    bool    _is_loadable = false;
    bool    _allow_mode_changing = true;

public:
    explicit CTabScreen(QWidget *parent = nullptr);

    virtual const QList<QMenu*> menus(QWidget *parent = nullptr) const = 0;

    bool isSavable () const;
    bool isLoadable() const;
    bool isModeChangeAllowed() const;

    virtual QString     getName() = 0;
    virtual CTabScreen* makeNew(QWidget *parent = nullptr) const = 0;

signals:
    void fileLoaded (const QString& file="");
    void fileSaved  (const QString& file="");
    void fileError  (const QString& file="", const QString& error="");

    void dataChanged() const;
    void fileUnsaved(const QString& file="") const;

    void savabilityChanged (bool savability ) const;
    void loadabilityChanged(bool loadability) const;
    void modeChanged       (bool mode       ) const;

public slots:
    virtual bool saveFile(const QString& file="") const = 0;
    virtual bool loadFile(const QString& file="") = 0;

    virtual void setActive  (QWidget* caller = nullptr) { };
    void setSavable (bool mode);
    void setLoadable(bool mode);
    void setModeChangeAllowed(bool mode);
};
