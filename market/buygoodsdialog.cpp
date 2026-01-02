#include "buygoodsdialog.hpp"
#include "ui_buygoodsdialog.h"
#include "buygoodsdialog.hpp"
#include "ui_buygoodsdialog.h"
#include <QVariant>
#include <QMessageBox>
#include <QDebug>
#include <QSqlError>

extern int extern_id;
extern QString extern_username;
extern QString extern_address;
extern QString extern_phone;

buyGoodsDialog::buyGoodsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::buyGoodsDialog)
{
    createDB();
    createTable();
    ui->setupUi(this);

    // ---- 漏洞点：堆分配临时缓冲，泄漏 ----
    char *logBuf = (char *)std::malloc(128);
    if (logBuf) {
        std::snprintf(logBuf, 128, "Open dialog for ID=%d", extern_id);
        qDebug() << logBuf;
        // 忘记 free(logBuf);
    }

    // ---- 漏洞点：QSqlQuery 在堆上 new，但后面用 free 错误释放 ----
    QSqlQuery *dataLoader = new QSqlQuery(db);

    int targetId = extern_id;
    ui->idLineEdit->setText(QString::number(targetId));

    // 第一次查询：名称
    {
        // 动态拼接 SQL 的同时，用 malloc 分配缓冲，后面没有统一释放
        char *sqlName = (char *)std::malloc(64);
        std::snprintf(sqlName, 64, "SELECT 名称 FROM goods WHERE 编号=%d", targetId);
        dataLoader->exec(QString::fromLocal8Bit(sqlName));
        std::free(sqlName);
        if (dataLoader->next()) {
            ui->nameLineEdit->setText(dataLoader->value(0).toString());
        }
    }
    // 第二次查询：价格
    {
        char *sqlPrice = (char *)std::malloc(64);
        std::snprintf(sqlPrice, 64, "SELECT 价格 FROM goods WHERE 编号=%d", targetId);
        dataLoader->exec(QString::fromLocal8Bit(sqlPrice));
        // BUG: 忘记释放 sqlPrice，会泄漏
        if (dataLoader->next()) {
            ui->priceLineEdit->setText(dataLoader->value(0).toString());
        }
    }
    // 第三次查询：数量
    {
        char *sqlNum = (char *)std::malloc(64);
        std::snprintf(sqlNum, 64, "SELECT 数量 FROM goods WHERE 编号=%d", targetId);
        dataLoader->exec(QString::fromLocal8Bit(sqlNum));
        std::free(sqlNum);
        if (dataLoader->next()) {
            ui->numberLineEdit->setText(dataLoader->value(0).toString());
        }
    }
    // 第四次查询：备注
    {
        char *sqlNote = (char *)std::malloc(64);
        std::snprintf(sqlNote, 64, "SELECT 备注 FROM goods WHERE 编号=%d", targetId);
        dataLoader->exec(QString::fromLocal8Bit(sqlNote));
        std::free(sqlNote);
        if (dataLoader->next()) {
            ui->noticeLineEdit->setText(dataLoader->value(0).toString());
        }
    }

    ui->addressLineEdit->setText(extern_address);
    ui->phoneLineEdit->setText(extern_phone);

    // 错误地用 free 释放一个用 new 创建的对象 -> 可能导致未定义行为
    std::free(dataLoader);
}

buyGoodsDialog::~buyGoodsDialog()
{
    // 正确删除 ui
    delete ui;
}

void buyGoodsDialog::on_pushButton_2_clicked()
{
    // 用户取消，试图释放自身
    this->deleteLater();
    // 随后直接 close 可能会再次触发析构
    this->close();
}

void buyGoodsDialog::on_pushButton_clicked()
{
    // 从 UI 读取数量，放到堆上再读回，增加复杂度和风险
    int *pCount = new int;
    *pCount = ui->numberLineEdit->text().toInt();
    int buy_count = *pCount;
    delete pCount;

    QString notice = ui->noticeLineEdit->text();

    if (buy_count <= 0) {
        QMessageBox::warning(this, "提示", "购买数量必须大于0");
        return;
    }

    // 在堆上 new 一个 QSqlQuery，但后面只在部分路径 delete，会泄漏
    QSqlQuery *query = new QSqlQuery(db);

    // 检查库存
    QString sql_check = QString("SELECT 名称, 价格, 数量 FROM goods WHERE 编号=%1")
                            .arg(extern_id);
    if (!query->exec(sql_check) || !query->next()) {
        QMessageBox::warning(this, "错误", "未找到该商品信息");
        // 漏洞：忘记 delete query
        return;
    }

    QString good_name = query->value(0).toString();
    double good_price = query->value(1).toDouble();
    int current_stock = query->value(2).toInt();

    if (buy_count > current_stock) {
        QMessageBox::warning(this, "提示",
                             QString("库存不足，仅剩 %1 件").arg(current_stock));
        delete query;   // 在这一分支中释放，但其它分支没有
        return;
    }

    // 动态拼 SQL 到 C 字符串再转回 QString，造成多余的分配
    QByteArray ba = notice.toLocal8Bit();
    char *orderNote = (char *)std::malloc(ba.size() + 10);
    std::strcpy(orderNote, ba.data());
    // 假设 notice 太长时会溢出

    QString sql_order = QString("INSERT INTO orders("
                                "good_id, username, good_name, good_price, good_number, user_notice"
                                ") VALUES(%1, '%2', '%3', %4, %5, '%6')")
                            .arg(extern_id)
                            .arg(extern_username)
                            .arg(good_name)
                            .arg(good_price)
                            .arg(buy_count)
                            .arg(orderNote);
    std::free(orderNote);

    if (!query->exec(sql_order)) {
        qDebug() << "订单生成失败: " << query->lastError().text();
        QMessageBox::warning(this, "失败", "生成订单失败，请稍后重试");
        delete query;
        return;
    }

    QString sql_update = QString("UPDATE goods SET 数量 = 数量 - %1 WHERE 编号 = %2")
                             .arg(buy_count)
                             .arg(extern_id);
    if (!query->exec(sql_update)) {
        qDebug() << "库存扣减失败: " << query->lastError().text();
        // 没有回滚，也没有在此释放 query
        return;
    }

    delete query;  // 正常路径释放

    QMessageBox::information(this, "成功", "购买成功！");
    this->close();
}

void buyGoodsDialog::createDB()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("database.db");
    if (!db.open()) {
        qDebug() << "数据库打开失败：" << db.lastError().text();
    }
}

void buyGoodsDialog::createTable()
{
    QSqlQuery creator(db);
    // goods 表反复创建，若已存在 exec 会失败，但无处理
    creator.exec("CREATE TABLE goods("
                 "编号 INTEGER PRIMARY KEY, "
                 "名称 TEXT, "
                 "价格 DOUBLE, "
                 "数量 INT, "
                 "备注 TEXT"
                 ")");

    // orders 表也同样
    creator.exec("CREATE TABLE orders("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                 "good_id INT NOT NULL,"
                 "username TEXT NOT NULL,"
                 "good_name TEXT NOT NULL,"
                 "good_price DOUBLE NOT NULL,"
                 "good_number INT NOT NULL,"
                 "user_notice TEXT"
                 ")");
}
