#include "menuwidget.hpp"
#include "ui_menuwidget.h"
#include "releasegoodsdialog.hpp"
#include "myprofiledialog.hpp"
#include "buygoodsdialog.hpp"
#include "myordersdialog.hpp"
#include "logindialog.hpp"

// 如果有全局变量引用
extern int extern_id;

MenuWidget::MenuWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MenuWidget)
{
    ui->setupUi(this);

    createDB();    // 1. 连接数据库
    createTable(); // 2. 确保表存在（使用统一的英文列名）
    queryTable();  // 3. 显示数据
}

MenuWidget::~MenuWidget()
{
    delete ui;
}

void MenuWidget::createDB()
{
    // 【修复】防止 "connection is still in use" 报错
    if(QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("database.db");
    }

    if (!db.isOpen()) {
        if (!db.open()) {
            qDebug() << "数据库打开失败:" << db.lastError().text();
        }
    }
}

void MenuWidget::createTable()
{
    QSqlQuery query;
    // 【核心】统一使用英文列名，避免乱码和不匹配问题
    QString str = "CREATE TABLE IF NOT EXISTS goods ("
                  "id INTEGER PRIMARY KEY, "
                  "name TEXT, "
                  "price REAL, "
                  "number INTEGER, "
                  "description TEXT, "
                  "seller TEXT)";

    if(!query.exec(str)){
        qDebug() << "建表失败:" << query.lastError().text();
    }
}

void MenuWidget::queryTable()
{
    // 1. 查询英文列名
    QString str = "SELECT id, name, price, number, description FROM goods";
    model.setQuery(str);

    // 2. 在界面上显示为中文
    model.setHeaderData(0, Qt::Horizontal, "编号");
    model.setHeaderData(1, Qt::Horizontal, "名称");
    model.setHeaderData(2, Qt::Horizontal, "价格");
    model.setHeaderData(3, Qt::Horizontal, "数量");
    model.setHeaderData(4, Qt::Horizontal, "备注");

    ui->table->setModel(&model);

    // 调试信息：如果行数为0，说明没数据或者查询失败
    // qDebug() << "当前查询到的行数:" << model.rowCount();
    if (model.lastError().isValid()) {
        qDebug() << "查询出错:" << model.lastError().text();
    }
}

// --- 以下是其他按钮的槽函数，保持逻辑不变 ---

void MenuWidget::on_releaseButton_clicked()
{
    ReleaseGoodsDialog *release = new ReleaseGoodsDialog("user");
    release->show();
    this->close();
}

void MenuWidget::on_myOrderButton_clicked()
{
    myOrdersDialog *myOrders = new myOrdersDialog;
    myOrders->show();
}

void MenuWidget::on_myinfoButton_clicked()
{
    myProfileDialog *profile = new myProfileDialog;
    profile->show();
}

void MenuWidget::on_buyButton_clicked()
{
    // 购买逻辑... (略，注意这里查询也要配合改成 id)
    extern_id = ui->buyIdEdit->text().toInt();
    QSqlQuery query;
    query.exec(QString("SELECT id FROM goods WHERE id=%1").arg(extern_id));
    if(!query.next()){
        QMessageBox::warning(this, "提示", "不存在该商品");
    } else {
        buyGoodsDialog *buy = new buyGoodsDialog;
        buy->show();
    }
}

void MenuWidget::on_orderButton_clicked()
{
    // 排序逻辑修复
    QString value = ui->speciesComboBox->currentText();
    // 将中文下拉框映射回英文列名
    if(value == "编号") value = "id";
    else if(value == "名称") value = "name";
    else if(value == "价格") value = "price";
    else if(value == "数量") value = "number";

    QString sort = (ui->howComboBox->currentIndex() == 0) ? "ASC" : "DESC";

    QString str = QString("SELECT id, name, price, number, description FROM goods ORDER BY %1 %2")
                      .arg(value).arg(sort);
    model.setQuery(str);

    // 重新设置表头
    model.setHeaderData(0, Qt::Horizontal, "编号");
    model.setHeaderData(1, Qt::Horizontal, "名称");
    model.setHeaderData(2, Qt::Horizontal, "价格");
    model.setHeaderData(3, Qt::Horizontal, "数量");
    model.setHeaderData(4, Qt::Horizontal, "备注");

    ui->table->setModel(&model);
}

void MenuWidget::on_searchButton_clicked()
{
    // 搜索逻辑修复：查询 name
    QString search = QString("SELECT id, name, price, number, description FROM goods "
                             "WHERE name LIKE '%%1%'").arg(ui->searchIdEdit->text());
    model.setQuery(search);

    model.setHeaderData(0, Qt::Horizontal, "编号");
    model.setHeaderData(1, Qt::Horizontal, "名称");
    model.setHeaderData(2, Qt::Horizontal, "价格");
    model.setHeaderData(3, Qt::Horizontal, "数量");
    model.setHeaderData(4, Qt::Horizontal, "备注");

    ui->table->setModel(&model);
}

void MenuWidget::on_RenewpushButton_clicked()
{
    ui->searchIdEdit->setText("");
    queryTable();
}

void MenuWidget::on_pushButton_clicked()
{
    LoginDialog *login = new LoginDialog;
    this->close();
    login->show();
}

