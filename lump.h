#ifndef LUMP_H
#define LUMP_H

#include <QString>

class Lump {
public:
    unsigned revision = 0;
    QString name = "Unknown LUMPITEM";
    QString type = "Unknown";
    QString path = "/unknown/";
    QString datapath = "";
    bool    link = false;
};

#endif // LUMP_H
