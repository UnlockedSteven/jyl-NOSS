#include "releasegoodsdialog.hpp"
#include "ui_releasegoodsdialog.h"
#include "menuwidget.hpp"
#include "mainwindow.h" // 假设管理员界面在这里
#include <QIntValidator>

// 全局变量引用
extern QString extern_username; // 当前登录用户名
extern QString extern_commodity; // 管理员操作的表名(如果有)
QString extern_from;
ReleaseGoodsDialog::ReleaseGoodsDialog(const QString &from, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReleaseGoodsDialog)
{
    ui->setupUi(this);
    extern_from = from; // 记录是从哪里来的 (admin 或 user)

    ui->idEdit->setValidator(new QIntValidator(this));
    ui->priceEdit->setValidator(new QDoubleValidator(this)); // 价格应该是 Double
    ui->numberEdit->setValidator(new QIntValidator(this));

    ui->releaseButton->setFocus();
    createDB();
}

ReleaseGoodsDialog::~ReleaseGoodsDialog()
{
    delete ui;
}

void ReleaseGoodsDialog::createDB()
{
    // 【修复】与 MenuWidget 保持一致的连接逻辑
    if(QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("database.db");
    }

    if (!db.isOpen()) {
        db.open();
    }
}

void ReleaseGoodsDialog::on_releaseButton_clicked()
{
    int _id = ui->idEdit->text().toInt();
    QString _name = ui->nameEdit->text();
    double _price = ui->priceEdit->text().toDouble();
    int _number = ui->numberEdit->text().toInt();
    QString _notice = ui->noticeEdit->text();

    // 简单防空校验
    if(_name.isEmpty()){
        QMessageBox::warning(this, "提示", "请完整填写商品信息");
        return;
    }

    QSqlQuery query;

    // 1. 查重 (用英文 id)
    QString search = QString("SELECT id FROM goods WHERE id=%1").arg(_id);
    query.exec(search);

    // 【修复】直接判断 next()，如果为 true 说明找到了重复的
    if(query.next()){
        QMessageBox::warning(this, "错误", "编号已存在！");
        return;
    }

    // 2. 插入数据 (用英文列名，必须与 MenuWidget createTable 对应)
    // 注意 VALUES 里的单引号，文本要加引号
    QString str = QString("INSERT INTO goods(id, name, price, number, description, seller) "
                          "VALUES(%1, '%2', %3, %4, '%5', '%6')")
                          .arg(_id)
                          .arg(_name)
                          .arg(_price)
                          .arg(_number)
                          .arg(_notice)
                          .arg(extern_username); // 必须确保 extern_username 有值

    if(query.exec(str)){
        QMessageBox::information(this, "成功", "商品发布成功！");

        // 3. 跳转逻辑
        if(extern_from == "admin"){
            // 假设管理员是返回 MainWindow
            MainWindow *mainwindow = new MainWindow(extern_commodity);
            mainwindow->show();
        } else {
            // 普通用户返回 MenuWidget
            MenuWidget *menu = new MenuWidget;
            menu->show();
        }
        this->close();
    }
    else {
        // 错误打印
        qDebug() << "发布失败 SQL:" << str;
        qDebug() << "错误原因:" << query.lastError().text();
        QMessageBox::critical(this, "失败", "数据库写入错误:" + query.lastError().text());
    }
}

void ReleaseGoodsDialog::on_cancelButton_clicked()
{
    if(extern_from == "admin"){
        MainWindow *mainwindow = new MainWindow(extern_commodity);
        mainwindow->show();
    } else {
        MenuWidget *menu = new MenuWidget;
        menu->show();
    }
    this->close();
}



