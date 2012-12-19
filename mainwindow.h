#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui/QMessageBox>
#include <QApplication>
#include <QListWidget>
#include <QPlainTextEdit>

#include "pop.h"
#include "structs.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_connectBtn_clicked();
    void mailFill(mail s_mail);
    void errorHandler(QString str);
    void on_mailList_activated(const QModelIndex &index);
    void updateTitle(int i);

private:
    Ui::MainWindow *ui;
    Pop * CPop;
    QList<QByteArray> model;
};

#endif // MAINWINDOW_H
