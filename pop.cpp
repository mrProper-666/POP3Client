#include "pop.h"

Pop::Pop(QObject *parent) :
    QObject(parent)
{
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));
}

bool Pop::connectToServer(QString server, QString user, QString psswd){
    s_loginVars.login = user;
    s_loginVars.psswd = psswd;
    s_loginVars.server = server;
    socket->connectToHost(s_loginVars.server, 110);
    e_stat = START;
}

void Pop::readData(){
    QByteArray array = socket->readAll();
    qDebug() << array;

    if (array.contains("+")) {
        if (e_stat == START) {
            sendLogin();
            return;
        }
        if (e_stat == USER) {
            sendPass();
            return;
        }
        if (e_stat == PASS) {
            QString str = array;
            getTops(str.section(" ", 1, 1).toInt());
            return;
        }
        if (e_stat == TOP) {
            QRegExp rx("From: (\\S+(\\s|\\t)\\S+ru>|\\S+\\s\\S{1,20}com>|\\S+\\s\\S{1,20}net>)");
            rx.indexIn(array);
            mail s_mail;
            qDebug() << "deb" << rx.cap(1);
            s_mail.from = getFrom(rx.cap(1));

            rx.setPattern("Subject: =\\?(.+\\?)=");
            rx.indexIn(array);
            s_mail.title = getSubject(rx.cap(1));

            qDebug() << "return" << s_mail.from << s_mail.title << count << mailList.count();
            emit mailFinished(&s_mail);
            return;
        }
    } else if (array.contains("-")) {
        emit error(array);
    }
}

void Pop::sendLogin(){
    socket->write("USER " + s_loginVars.login.toAscii() + "\r\n");
    e_stat = USER;
}

void Pop::sendPass(){
    socket->write("PASS " + s_loginVars.psswd.toAscii() + "\r\n");
    e_stat = PASS;
}

void Pop::getTops(int counter){
    e_stat = TOP;
    count = counter;
    for (int i = 1; i <= counter; i++) {
        QByteArray arr;
        arr.append("TOP ");
        arr.append(tr("%1 0").arg(i));
        arr.append("\r\n");
        socket->write(arr);
    }
}

QString Pop::getFrom(QString str){
    QRegExp rx("b\\?(.+)\\?=");
    rx.indexIn(str);
    QByteArray arr;
    arr.append(rx.cap(1));

    rx.setPattern("<(.+)>");
    rx.indexIn(str);

    return QString::fromUtf8(QByteArray::fromBase64(arr)) + " " + rx.cap(1);
}

QString Pop::getSubject(QString str){
    QByteArray arr;
    QString string;
    QRegExp rx;
    if (str.contains(" ")) {
        rx.setPattern("(.+\\?.{1,1}\\?)(.+)=*\\?\\1(.+)\\?");
    } else {
        rx.setPattern("(.+\\?.{1,1}\\?)(.+)\\?");
    }
    rx.indexIn(str);

    arr += rx.cap(2);
    string = QString::fromUtf8(QByteArray::fromBase64(arr));

    arr.clear();
    arr += rx.cap(3);
    string += QString::fromUtf8(QByteArray::fromBase64(arr));

    return string;
}
