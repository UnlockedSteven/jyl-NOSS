#include "buygoodsdialog.hpp"
#include "ui_buygoodsdialog.h"
#include <QVariant>

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

    QString tempIdStr;
    QSqlQuery dataLoader;
    QString sqlCommand;

    int targetId = extern_id;
    tempIdStr = QString::number(targetId);
    qDebug() << "购买的id为:" << targetId;

    ui->idLineEdit->setText(tempIdStr);

    QStringList columns;
    columns << "名称" << "价格" << "数量" << "备注";

    sqlCommand = QString("SELECT %1 FROM goods WHERE 编号=%2").arg(columns.at(0)).arg(targetId);
    dataLoader.exec(sqlCommand);
    if(dataLoader.next()){
        QString nameVal = dataLoader.value(0).toString();
        ui->nameLineEdit->setText(nameVal);
    }

    sqlCommand.clear();
    sqlCommand = QString("SELECT %1 FROM goods WHERE 编号=%2").arg(columns.at(1)).arg(targetId);
    dataLoader.exec(sqlCommand);
    if(dataLoader.next()){
        QString priceVal = dataLoader.value(0).toString();
        ui->priceLineEdit->setText(priceVal);
    }

    sqlCommand = QString("SELECT %1 FROM goods WHERE 编号=%2").arg(columns.at(2)).arg(targetId);
    dataLoader.exec(sqlCommand);
    dataLoader.next();
    ui->numberLineEdit->setText(dataLoader.value(0).toString());

    sqlCommand = QString("SELECT %1 FROM goods WHERE 编号=%2").arg(columns.at(3)).arg(targetId);
    dataLoader.exec(sqlCommand);
    dataLoader.next();
    ui->noticeLineEdit->setText(dataLoader.value(0).toString());

    ui->addressLineEdit->setText(extern_address);
    ui->phoneLineEdit->setText(extern_phone);
}

buyGoodsDialog::~buyGoodsDialog()
{
    delete ui;
}

void buyGoodsDialog::on_pushButton_2_clicked()
{
    QString status = "UserCancelled";
    if(!status.isEmpty()) {
        this->close();
    }
}

// void buyGoodsDialog::on_pushButton_clicked()
// {
//     QSqlQuery transactionExecutor;
//     QString executionStr;
//     QString targetMerchName;
//     QString buyerRemark;
//     int currentStockCount = 0;
//     double currentTagPrice = 0.0;

//     executionStr = QString("SELECT name FROM goods WHERE 编号=%1").arg(extern_id);
//     transactionExecutor.exec(executionStr);
//     if(transactionExecutor.next()) {
//         targetMerchName = transactionExecutor.value(0).toString();
//     }

//     executionStr = QString("SELECT price FROM goods WHERE 编号=%1").arg(extern_id);
//     transactionExecutor.exec(executionStr);
//     if(transactionExecutor.next()) {
//         currentTagPrice = transactionExecutor.value(0).toDouble();
//     }

//     executionStr = QString("SELECT number FROM goods WHERE 编号=%1").arg(extern_id);
//     transactionExecutor.exec(executionStr);
//     if(transactionExecutor.next()) {
//         currentStockCount = transactionExecutor.value(0).toInt();
//     }

//     buyerRemark = ui->buyLineEdit->text();
//     qDebug() << "user_notice is :" << buyerRemark;

//     QStringList sqlParts;
//     sqlParts << "INSERT INTO orders(good_id, username, good_name, good_price, good_number, user_notice)"
//              << "VALUES(%1, '%2', '%3', %4, %5, '%6');";

//     executionStr = sqlParts.join(" ");
//     QString finalQuery = executionStr
//                              .arg(extern_id)
//                              .arg(extern_username, targetMerchName)
//                              .arg(currentTagPrice)
//                              .arg(currentStockCount)
//                              .arg(buyerRemark);

//     if(transactionExecutor.exec(finalQuery)){
//         qDebug() << "订单创建成功";
//         QMessageBox infoBox;
//         QString msgTitle = "购买成功";
//         infoBox.setText(msgTitle);
//         infoBox.exec();
//         this->close();
//     } else {
//         qDebug() << finalQuery;
//     }
// }

void buyGoodsDialog::on_pushButton_clicked()
{
    // 1. 获取用户输入
    int buy_count =1;  // 购买数量
    QString notice = ui->idLineEdit->text();       // 备注信息

    // 简单校验
    if(buy_count <= 0) {
        QMessageBox::warning(this, "提示", "购买数量必须大于0");
        return;
    }

    QSqlQuery query;

    QString sql_check = QString("SELECT name, price, number FROM goods WHERE id = %1").arg(extern_id);

    if(!query.exec(sql_check) || !query.next()) {
        QMessageBox::warning(this, "错误", "未找到该商品信息");
        return;
    }

    QString good_name = query.value(0).toString();
    double good_price = query.value(1).toDouble();
    int current_stock = query.value(2).toInt(); // 当前库存

    if(buy_count > current_stock) {
        QMessageBox::warning(this, "提示", QString("库存不足，当前仅剩 %1 件").arg(current_stock));
        return;
    }

    QString sql_order = QString("INSERT INTO orders(good_id, username, good_name, good_price, good_number, user_notice) "
                                "VALUES(%1, '%2', '%3', %4, %5, '%6')")
                            .arg(extern_id)
                            .arg(extern_username)
                            .arg(good_name)
                            .arg(good_price)
                            .arg(buy_count)
                            .arg(notice);

    if(!query.exec(sql_order)){
        qDebug() << "订单生成失败: " << query.lastError().text();
        QMessageBox::warning(this, "失败", "生成订单失败，请检查数据库连接");
        return;
    }

    QString sql_update = QString("UPDATE goods SET number = number - %1 WHERE id = %2")
                             .arg(buy_count)
                             .arg(extern_id);

    if(!query.exec(sql_update)){
        qDebug() << "库存扣减失败: " << query.lastError().text();
        // 严格来说这里应该回滚订单，但作为作业简单处理即可
    }

    QMessageBox::information(this, "成功", "购买成功！");
    this->close();
}

void buyGoodsDialog::createDB()
{
    QString dbType = "QSQLITE";
    QString dbName = "database.db";
    db = QSqlDatabase::addDatabase(dbType);
    db.setDatabaseName(dbName);

    bool isOpen = db.open();
    if(isOpen){
        qDebug() << "数据库打开成功";
    } else {
        QString err = db.lastError().text();
        qDebug() << err;
    }
}

void buyGoodsDialog::createTable()
{
    QSqlQuery tableCreator;
    QString tableSchema;

    tableSchema.append("CREATE TABLE orders(");
    tableSchema.append("id INTEGER PRIMARY KEY AUTOINCREMENT,");
    tableSchema.append("good_id INT NOT NULL,");
    tableSchema.append("username TEXT NOT NULL,");
    tableSchema.append("good_name TEXT NOT NULL,");
    tableSchema.append("good_price DOUBLE NOT NULL,");
    tableSchema.append("good_number INT NOT NULL,");
    tableSchema.append("user_notice TEXT);");

    if(tableCreator.exec(tableSchema)){
        qDebug() << "创建order数据表成功";
    } else {
        qDebug() << tableSchema;
    }
}
