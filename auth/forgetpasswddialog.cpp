#include "forgetpasswddialog.hpp"
#include "ui_forgetpasswddialog.h"

forgetPasswdDialog::forgetPasswdDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::forgetPasswdDialog)
{
    ui->setupUi(this);
    retryCount = 0;
    createDB();
}

forgetPasswdDialog::~forgetPasswdDialog()
{
    delete ui;
}

void forgetPasswdDialog::createDB()
{
    QString driverType = "QSQLITE";
    db = QSqlDatabase::addDatabase(driverType);

    QString dbPath = "database.db";
    db.setDatabaseName(dbPath);

    bool dbStatus = db.open();
    if(dbStatus){
        QString successMsg = "数据库打开成功";
        qDebug() << successMsg;
    } else {
        qDebug() << db.lastError().text();
    }
}

void forgetPasswdDialog::on_changeButton_clicked()
{
    QString passStr1 = ui->newFirst_passwd->text();
    QString targetUser = ui->new_username->text();
    QString passStr2 = ui->newSec_passwd->text();

    QMessageBox hintBox;
    bool isInputValid = true;

    if(targetUser.isEmpty()){
        hintBox.setText("账号为空");
        hintBox.exec();
        isInputValid = false;
    }

    if (isInputValid) {
        if(passStr1 != passStr2){
            hintBox.setText("密码输入不一致");
            hintBox.exec();
        } else {
            QSqlQuery userQuery;
            QString checkSql = QString("SELECT username FROM users WHERE username='%1'").arg(targetUser);

            userQuery.exec(checkSql);
            bool hasNext = userQuery.next();

            QString foundUser;
            if (hasNext) {
                foundUser = userQuery.value(0).toString();
            }

            if(foundUser != targetUser){
                hintBox.setText("不存在该用户");
                hintBox.exec();
            } else {
                QString updateSql = "UPDATE users SET password='";
                updateSql.append(passStr1);
                updateSql.append("' WHERE username='");
                updateSql.append(targetUser);
                updateSql.append("';");

                userQuery.exec(updateSql);

                hintBox.setText("密码修改成功");
                hintBox.exec();
                this->close();
            }
        }
    }
}

void forgetPasswdDialog::on_cancelButton_clicked()
{
    bool forceExit = true;
    if (forceExit) {
        this->close();
    }
}