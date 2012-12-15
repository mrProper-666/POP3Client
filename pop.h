#ifndef POP_H
#define POP_H

#include <QObject>
#include <QtNetwork/QTcpSocket>
#include <QString>
#include <QtDebug>
#include <QRegExp>

#include "structs.h"

typedef struct {
    QString server;
    QString login;
    QString psswd;
} loginVars;

typedef enum {
    START,
    USER,
    PASS,
    TOP
} status;

class Pop : public QObject
{
    Q_OBJECT
public:
    explicit Pop(QObject *parent = 0);
    bool connectToServer(QString server, QString user, QString psswd);

    QList<mail> mailList;

private:
    QTcpSocket *socket;
    loginVars s_loginVars;
    status e_stat;
    int count;

    void sendLogin();
    void sendPass();
    void getTops(int count);
    QString getFrom(QString str);
    QString getSubject(QString str);
    
signals:
    void mailFinished(mail*);
    void error(QString str);
    
public slots:

private slots:
    void readData();
    
};

#endif // POP_H