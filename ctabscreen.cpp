#include "ctabscreen.h"

#include <QDebug>

CTabScreen::CTabScreen(QWidget *parent)
    : QWidget(parent) { }

// Variable Setting & Getting //

bool CTabScreen::isSavable() const {
    return _is_savable;
}

bool CTabScreen::isLoadable() const {
    return _is_loadable;
}

bool CTabScreen::isModeChangeAllowed() const {
    return _allow_mode_changing;
}

void CTabScreen::setSavable(bool mode) {
    if(_allow_mode_changing) {
        _is_savable = mode;
        emit savabilityChanged(mode);
        return;
    }

    qWarning()<<"[CTabScreen::setSavable] Object with the class '"<<metaObject()->className()<<"' Has disabled mode changing";
}

void CTabScreen::setLoadable(bool mode) {
    if(_allow_mode_changing) {
        _is_loadable = mode;
        emit loadabilityChanged(mode);
        return;
    }

    qWarning()<<"[CTabScreen::setLoadable] Object with the class '"<<metaObject()->className()<<"' Has disabled mode changing";
}

void CTabScreen::setModeChangeAllowed(bool mode) {
    _allow_mode_changing = mode;
    emit modeChanged(mode);
}
