#ifndef LOGINDIALOG_HPP
#define LOGINDIALOG_HPP

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QString>
#include <QSqlRecord>
#include <QPointer>
#include <QBitArray>
#include <QBasicTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class LoginDialog; }
QT_END_NAMESPACE

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private slots:
    void on_loginButton_clicked();
    void on_forgetButton_clicked();
    void on_registerButton_clicked();

private:
    void createTable();
    void createDB();

    Ui::LoginDialog *ui;
    QSqlDatabase db;
    int attemptCounter;
    bool isSystemReady;
};

#endif