#ifndef STRUCTS_H
#define STRUCTS_H

#include <QString>
#include <QByteArray>

typedef struct {
    QString from;
    QString title;
    QByteArray mime;
} mail;

#endif // STRUCTS_H
