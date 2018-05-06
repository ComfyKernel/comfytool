#ifndef LUMP_H
#define LUMP_H

#include <QString>

struct Lump {
public:
    QString name;
    QString type;
    QString path;
    QString data;

    unsigned int versionMajor;
    unsigned int versionMinor;
};

#endif // LUMP_H
