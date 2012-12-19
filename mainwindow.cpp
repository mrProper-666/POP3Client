#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(qApp->applicationName());
    CPop = new Pop;
    connect(CPop, SIGNAL(mailFinished(mail)),this,SLOT(mailFill(mail)));
    connect(CPop, SIGNAL(error(QString)), this, SLOT(errorHandler(QString)));
    connect(CPop, SIGNAL(mailCount(int)), this, SLOT(updateTitle(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete CPop;
}

void MainWindow::on_connectBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    CPop->connectToServer(ui->addrLine->text(),
                          ui->userLine->text(),
                          ui->psswdLine->text());
}

void MainWindow::mailFill(mail s_mail){
    ui->mailList->insertItem(ui->mailList->count()+1, QString("От: %1\nТема: %2").arg(s_mail.from).arg(s_mail.title));
    model << s_mail.mime;
}

void MainWindow::errorHandler(QString str){
    QMessageBox::critical(this, qApp->applicationName(), str, QMessageBox::Ok);
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_mailList_activated(const QModelIndex &index)
{
    QPlainTextEdit * edit = new QPlainTextEdit(model.value(index.row()));
    edit->setReadOnly(true);
    edit->show();
}

void MainWindow::updateTitle(int i){
    setWindowTitle(QString("%1 %2 сообщений").arg(windowTitle()).arg(i));
}
