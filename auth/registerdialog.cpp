#include "registerdialog.hpp"
#include "ui_registerdialog.h"
#include <QCoreApplication>
RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);

    QString pattern = "[0-9]*";
    QRegularExpression regExp(pattern);
    QValidator *phoneVal = new QRegularExpressionValidator(regExp, this);
    ui->phoneEdit_2->setValidator(phoneVal);

    createDB();
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::createDB()
{
    QString dbPath = QCoreApplication::applicationDirPath() + "/database.db";

    if(QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE");
    }

    db.setDatabaseName(dbPath); // 强制设置路径

    if (!db.isOpen()) {
        if(db.open()){
            qDebug() << "注册界面-数据库连接成功";
        } else {
            qDebug() << "注册界面-数据库失败: " << db.lastError().text();
        }
    }
}

void RegisterDialog::on_cancelButton_clicked()
{
    this->close();
}

void RegisterDialog::on_registerButton_clicked()
{
    QString userNameStr = ui->new_username->text();
    QString pwd1Str = ui->newFirst_passwd->text();
    QString pwd2Str = ui->newSec_passwd->text();
    QString phoneStr = ui->phoneEdit_2->text();
    QString addrStr = ui->addressEdit->text();

    // 基础非空检查
    if(userNameStr.isEmpty() || addrStr.isEmpty() ||
        pwd1Str.isEmpty() || pwd2Str.isEmpty() || phoneStr.isEmpty())
    {
        QMessageBox infoBox;
        infoBox.setText("信息请填写完整! ");
        infoBox.exec();
        return;
    }

    // 密码一致性检查
    if(pwd1Str != pwd2Str)
    {
        QMessageBox mismatchBox;
        mismatchBox.setText("两次密码输入不一致");
        mismatchBox.exec();
        return;
    }

    // 检查用户是否存在
    QSqlQuery checkQuery;
    QString searchSql = QString("SELECT username FROM users WHERE username='%1'").arg(userNameStr);

    checkQuery.exec(searchSql);

    bool userExists = false;
    if(checkQuery.next()){
        if(checkQuery.value(0).toString() == userNameStr){
            userExists = true;
        }
    }

    if(userExists)
    {
        QMessageBox existBox;
        existBox.setText("该用户名已存在! ");
        existBox.exec();
    }
    else
    {
        // 执行注册
        QString insertSql = QString("INSERT INTO users(username, password, phone, address) VALUES('%1', '%2', '%3', '%4')")
                                .arg(userNameStr)
                                .arg(pwd1Str)
                                .arg(phoneStr)
                                .arg(addrStr);

        QSqlQuery insertQuery;
        if(insertQuery.exec(insertSql))
        {
            QMessageBox successBox;
            successBox.setText("注册成功");
            successBox.exec();
            this->close();
        }
        else
        {
            qDebug() << "Registration failed:" << insertSql;
        }
    }
}