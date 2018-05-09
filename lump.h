#ifndef LUMP_H
#define LUMP_H

#include <QString>

class Lump {
public:
    unsigned revision;
    QString name;
    QString type;
    QString path;
    QString datapath;
};

#endif // LUMP_H
