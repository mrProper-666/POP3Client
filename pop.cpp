#include "pop.h"

Pop::Pop(QObject *parent) :
    QObject(parent)
{
    socket = new QTcpSocket(this);
}

bool Pop::connectToServer(QString server, QString user, QString psswd){
    s_loginVars.login = user;
    s_loginVars.psswd = psswd;
    s_loginVars.server = server;
    socket->connectToHost(s_loginVars.server, 110);

    QByteArray arr;
    while(socket->waitForReadyRead(1000))
    {
        arr.insert(arr.length(), socket->readAll());
        qDebug() << arr.data();
        qDebug() << "Length: " << arr.length();
        if(arr.endsWith(".\r\n"))
            break;
    }
    sendLogin();
    e_stat = START;
}

void Pop::sendLogin(){
    socket->write("USER " + s_loginVars.login.toAscii() + "\r\n");
    e_stat = USER;
    QByteArray arr;
    while(socket->waitForReadyRead(1000))
    {
        arr.insert(arr.length(), socket->readAll());
        qDebug() << arr.data();
        qDebug() << "Length: " << arr.length();
        if(arr.endsWith(".\r\n"))
            break;
    }

    sendPass();
}

void Pop::sendPass(){
    socket->write("PASS " + s_loginVars.psswd.toAscii() + "\r\n");

    QByteArray arr;
    while(socket->waitForReadyRead(1000))
    {
        arr.insert(arr.length(), socket->readAll());
        qDebug() << arr.data();
        qDebug() << "Length: " << arr.length();
        if(arr.endsWith(".\r\n"))
            break;
    }
    if (arr.startsWith("-")) {
        emit error(arr);
        return;
    }

    QString str = arr;
    getTops(str.section(" ", 1, 1).toInt());

    e_stat = PASS;
}

void Pop::getTops(int counter){
    emit mailCount(counter);
    e_stat = TOP;
    count = counter;
    for (int i = 1; i <= counter; i++) {
        QByteArray arr;
        arr.append("TOP ");
        arr.append(tr("%1 0").arg(i));
        arr.append("\r\n");
        socket->write(arr);

        arr.clear();
        while(socket->waitForReadyRead(2000))
        {
            arr.insert(arr.length(), socket->readAll());
            qDebug() << arr.data();
            qDebug() << "Length: " << arr.length();
            if(arr.endsWith(".\r\n"))
                break;
        }

        QRegExp rx("From: (.{1,100}\\>)\\r\\n");
        rx.indexIn(arr);
        mail s_mail;
        qDebug() << "deb" << rx.cap(1);
        s_mail.mime = arr;
        s_mail.from = getFrom(rx.cap(1));

        rx.setPattern("Subject: =\\?([^:]+\\?)=\\r\\n");
        rx.indexIn(arr);
        qDebug() << "top" << rx.cap(1) << rx.cap(0);
        s_mail.title = getSubject(rx.cap(1));

        qDebug() << "return" << s_mail.from << s_mail.title << count << mailList.count();
        emit mailFinished(s_mail);
    }
}

QString Pop::getFrom(QString str){
    QRegExp rx("=\\?(utf\\-8|UTF\\-8|koi8\\-r|KOI8\\-R|windows\\-1251|WINDOWS\\-1251).{3,3}(\\d.+)\\?=\\s");
    rx.indexIn(str.section(" ", 0));
    qDebug() << "rx" << rx.cap(1) << rx.cap(2);

    QString ret = fromBase64(rx.cap(2), rx.cap(1));
    rx.setPattern("(<.+>)");
    rx.indexIn(str);

    return ret + " " + rx.cap(1);
}

QString Pop::getSubject(QString str){
    QString string;
    QRegExp rx;
    qDebug() << "str" << str;
    if (str.contains(" ")) {
        rx.setPattern("(utf\\-8|UTF\\-8|koi8\\-r|KOI8\\-R|windows\\-1251|WINDOWS\\-1251).{3,3}(\\d.+)=\\?=\\s+=\\?(utf\\-8|UTF\\-8|koi8\\-r|KOI8\\-R|windows\\-1251|WINDOWS\\-1251).{3,3}(\\d.+)\\?");
        rx.indexIn(str);
        string = fromBase64(rx.cap(2), rx.cap(1)) + fromBase64(rx.cap(4), rx.cap(3));
    } else {
        rx.setPattern("(utf\\-8|UTF\\-8|koi8\\-r|KOI8\\-R|windows\\-1251|WINDOWS\\-1251).{3,3}(\\d.+)\\?");
        rx.indexIn(str);
        string = fromBase64(rx.cap(2), rx.cap(1));
    }

    return string;
}

QString Pop::fromBase64(QString string, QString encoding){
    qDebug() << "encoding" << encoding;
    if (encoding.toLower() == "utf-8")
        return QString::fromUtf8(QByteArray::fromBase64(qPrintable(string)));

    if (encoding.toLower() == "koi8-r") {
        QTextCodec *codec = QTextCodec::codecForName("KOI8-R");
        return codec->toUnicode(QByteArray::fromBase64(qPrintable(string)));
    }

    if (encoding.toLower() == "windows-1251") {
        QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
        return codec->fromUnicode(QByteArray::fromBase64(qPrintable(string)));
    }
    return tr("Err");
}
