#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    CPop = new Pop;
    connect(CPop, SIGNAL(mailFinished(mail*)),this,SLOT(mailFill(mail*)));
    connect(CPop, SIGNAL(error(QString)), this, SLOT(errorHandler(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete CPop;
}

void MainWindow::on_connectBtn_clicked()
{
    CPop->connectToServer(ui->addrLine->text(),
                          ui->userLine->text(),
                          ui->psswdLine->text());
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::mailFill(mail *s_mail){
    ui->mailList->insertItem(ui->mailList->count()+1, QString("От: %1\nТема: %2").arg(s_mail->from).arg(s_mail->title));
}

void MainWindow::errorHandler(QString str){
    QMessageBox::critical(this, qApp->applicationName(), str, QMessageBox::Ok);
    ui->stackedWidget->setCurrentIndex(0);
}
