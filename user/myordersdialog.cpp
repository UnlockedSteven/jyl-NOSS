#include "myordersdialog.hpp"
#include "ui_myordersdialog.h"
#include <QStringList>
#include <QColor>

extern QString extern_username;

myOrdersDialog::myOrdersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::myOrdersDialog)
{
    isInitialized = false;
    createDB();
    ui->setupUi(this);

    QString currentUser = extern_username;
    QStringList selectFields;
    selectFields << "good_id AS 商品编号"
                 << "good_name AS 商品名称"
                 << "good_price AS 商品价格"
                 << "good_number AS 商品库存"
                 << "user_notice AS 我的备注";

    QString fieldStr = selectFields.join(",");

    QString queryStr = QString("SELECT %1 FROM orders WHERE username='%2'")
                           .arg(fieldStr)
                           .arg(currentUser);

    qDebug() << "Querying orders: " << queryStr;

    model.setQuery(queryStr);

    if (model.lastError().isValid()) {
        qDebug() << model.lastError();
    } else {
        ui->tableView->setModel(&model);
        ui->tableView->setAlternatingRowColors(true);
    }

    isInitialized = true;
}

myOrdersDialog::~myOrdersDialog()
{
    if(ui){
        delete ui;
    }
}

void myOrdersDialog::createDB()
{
    QString sqlDriver = "QSQLITE";
    QString dbFile = "database.db";

    if(QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        db = QSqlDatabase::addDatabase(sqlDriver);
        db.setDatabaseName(dbFile);
    }

    bool connectionResult = db.open();

    if(connectionResult){
        QString msg = "数据库打开成功";
        qDebug() << msg;
    } else {
        QSqlError err = db.lastError();
        qDebug() << err.text();
    }
}