#include "logindialog.hpp"
#include "ui_logindialog.h"
#include "menuwidget.hpp"
#include "registerdialog.hpp"
#include "forgetpasswddialog.hpp"
#include "chosetabledialog.hpp"
#include <QCoreApplication> // 用于获取当前路径


extern QString extern_username;

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    // 默认选中用户模式
    ui->radioButton->setChecked(true);

    // 设置密码输入模式为原点
    ui->password->setEchoMode(QLineEdit::Password);

    createDB();
    createTable();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::createDB()
{
    // 1. 获取数据库绝对路径
    QString dbPath = QCoreApplication::applicationDirPath() + "/database.db";

    // 2. 智能连接：如果已存在默认连接则复用，否则新建
    if(QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE");
    }

    // 3. 确保路径设置正确
    db.setDatabaseName(dbPath);

    // 4. 打开数据库
    if (!db.isOpen()) {
        if(db.open()){
            qDebug() << "登录界面-数据库连接成功: " << dbPath;
        } else {
            qDebug() << "登录界面-数据库连接失败: " << db.lastError().text();
            QMessageBox::critical(this, "错误", "无法连接数据库文件!");
        }
    }
}

void LoginDialog::createTable()
{
    QSqlQuery query;

    // 创建用户表
    // id(PK), username(TEXT), password(TEXT), phone(TEXT), address(TEXT)
    QString str = "CREATE TABLE IF NOT EXISTS users("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "username TEXT NOT NULL UNIQUE, " // 增加UNIQUE约束
                  "password TEXT NOT NULL, "
                  "phone TEXT, "
                  "address TEXT);";

    if(!query.exec(str)){
        qDebug() << "创建用户表失败或表已存在: " << query.lastError();
    }

    // 创建商品表 (以防万一后续模块需要)
    // QString goodsTable = "CREATE TABLE IF NOT EXISTS goods("
    //                      "编号 INTEGER PRIMARY KEY, "
    //                      "名称 TEXT, "
    //                      "价格 REAL, "
    //                      "数量 INTEGER, "
    //                      "描述 TEXT, "
    //                      "卖家 TEXT);";
    QString goodsTable = "CREATE TABLE IF NOT EXISTS goods ("
                  "id INTEGER PRIMARY KEY, "
                  "name TEXT, "  // 绝对不能是 "名称"
                  "price REAL, "
                  "number INTEGER, "
                  "description TEXT, "
                  "seller TEXT)";
    query.exec(goodsTable);

    // 检查并创建默认管理员: admin / 123456
    QString search = "SELECT username FROM users WHERE username='admin'";
    if(query.exec(search) && query.next()){
        // admin已存在，不做操作
    } else {
        QString insertAdmin = "INSERT INTO users(username, password, phone, address) "
                              "VALUES('admin', '123456', '18888888888', 'System Administrator')";
        if(query.exec(insertAdmin)){
            qDebug() << "初始管理员账户创建成功";
        }
    }
}

void LoginDialog::on_loginButton_clicked()
{
    QString input_password = ui->password->text();
    QString input_username = ui->username->text();


    if(input_username == ""){
        QMessageBox msg;
        msg.setText("账号为空");
        msg.exec();
    }
    else if(input_password == ""){
        QMessageBox msg;
        msg.setText("密码为空");
        msg.exec();
    }
    // 账号密码都非空
    else{
        QSqlQuery query;
        QString str = QString("SELECT password FROM users WHERE username='%1'").arg(input_username);
        if(!query.exec(str)){
            qDebug() << str;
        }
        /**/
        query.next();
        qDebug() << "密码是:" << query.value(0).toString();
        /**/
        // 账号密码正确
        if(input_password == query.value(0).toString()){
            extern_username = input_username;
            if(input_username == "admin"){
                // 生成管理员后台
                choseTableDialog *choose = new choseTableDialog;
                this->close();
                choose->show();
            }
            else{
                // 生成用户界面主菜单
                MenuWidget *menu = new MenuWidget;
                QMessageBox msg;
                msg.setText("登陆成功");
                msg.exec();
                menu->show();
                this->close();
            }
        }
        // 账号或密码不正确
        else{
            QMessageBox msg;
            msg.setText("账号或密码错误");
            msg.exec();
        }
    }
}


void LoginDialog::on_registerButton_clicked()
{
    RegisterDialog *regDlg = new RegisterDialog;
    regDlg->show();
    // 这里不关闭 LoginDialog，方便注册完回来登录
}

void LoginDialog::on_forgetButton_clicked()
{
    forgetPasswdDialog *forgetDlg = new forgetPasswdDialog;
    forgetDlg->show();
}
