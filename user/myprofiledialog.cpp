#include "myprofiledialog.hpp"
#include "ui_myprofiledialog.h"
#include <QCoreApplication>
#include <QFileInfo>
extern QString extern_username;
extern QString extern_address;
extern QString extern_phone;

myProfileDialog::myProfileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::myProfileDialog)
{
    ui->setupUi(this);
    editModeEnabled = false;

    QIntValidator *phoneValidator = new QIntValidator(this);
    ui->phoneEdit->setValidator(phoneValidator);

    createDB();
    InitInfo();
}

myProfileDialog::~myProfileDialog()
{
    delete ui;
}

void myProfileDialog::createDB()
{
    // 1. 获取数据库的绝对路径
    QString dbPath = QCoreApplication::applicationDirPath() + "/database.db";

    // 2. 检查默认连接是否存在
    if(QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(dbPath);
    }

    // 3. 再次确认数据库名设置正确（防止复用连接时路径丢失）
    if(db.databaseName() != dbPath){
        db.setDatabaseName(dbPath);
    }

    // 4. 智能打开：如果已经打开就不需要再open，否则尝试open
    if (!db.isOpen()) {
        if(db.open()){
            qDebug() << "数据库连接成功: " << dbPath;
        } else {
            qDebug() << "数据库打开失败: " << db.lastError().text();
        }
    }
}

void myProfileDialog::InitInfo()
{
    QString currentUser = extern_username;
    QString currentPhone = extern_phone;
    QString currentAddr = extern_address;

    ui->usernameEdit->setText(currentUser);
    ui->phoneEdit->setText(currentPhone);
    ui->addressEdit->setText(currentAddr);
}

void myProfileDialog::on_changeButton_clicked()
{
    QString currentBtnText = ui->changeButton->text();
    QString editLabel = "修改";

    // 激活编辑模式
    if(currentBtnText == editLabel){
        ui->changeButton->setText("确定");

        ui->phoneEdit->setEnabled(true);
        ui->addressEdit->setEnabled(true);
        editModeEnabled = true;
    }
    // 提交更改
    else{
        QString newPhoneVal = ui->phoneEdit->text();
        QString newAddrVal = ui->addressEdit->text();

        QString updateSql = QString("UPDATE users SET phone='%1', address='%2' WHERE username='%3'")
                                .arg(newPhoneVal)
                                .arg(newAddrVal)
                                .arg(extern_username);

        QSqlQuery updateQuery;
        bool executionResult = updateQuery.exec(updateSql);

        if(executionResult){
            qDebug() << "信息更新成功";

            QMessageBox successMsg;
            successMsg.setText("信息修改成功");
            successMsg.exec();

            // 更新全局变量（如果需要的话，这里可以同步更新extern变量）
            extern_phone = newPhoneVal;
            extern_address = newAddrVal;

            this->close();
        } else {
            qDebug() << "信息修改失败:" << updateSql;
        }
    }
}