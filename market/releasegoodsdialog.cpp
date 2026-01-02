
#include "releasegoodsdialog.hpp"
#include "ui_releasegoodsdialog.h"
#include "menuwidget.hpp"
#include "mainwindow.h"
#include <QIntValidator>
#include <QDoubleValidator>
#include <QMessageBox>
#include <QDebug>
#include <QSqlError>
#include <cstdlib>
#include <cstdio>
extern QString extern_username;
extern QString extern_commodity;
extern QString extern_from;

ReleaseGoodsDialog::ReleaseGoodsDialog(const QString &from, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReleaseGoodsDialog)
{
    ui->setupUi(this);
    extern_from = from;

    // 漏洞1：给日志分配缓冲但从不释放 -> 内存泄漏
    char *logBuf = (char*)std::malloc(128);
    std::snprintf(logBuf, 128, "ReleaseGoodsDialog 初始化, 来源=%s", extern_from.toLocal8Bit().data());
    qDebug() << logBuf;
    // 忘记 std::free(logBuf);

    ui->idEdit->setValidator(new QIntValidator(this));
    ui->priceEdit->setValidator(new QDoubleValidator(this));
    ui->numberEdit->setValidator(new QIntValidator(this));

    createDB();
}

ReleaseGoodsDialog::~ReleaseGoodsDialog()
{
    delete ui;

}

void ReleaseGoodsDialog::createDB()
{
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
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
    QString _name     = ui->nameEdit->text();
    double _price     = ui->priceEdit->text().toDouble();
    int _number       = ui->numberEdit->text().toInt();
    QString _notice   = ui->noticeEdit->text();

    if (_name.isEmpty()) {
        QMessageBox::warning(this, "提示", "请完整填写商品信息");
        return;
    }

    // 漏洞3：new 了 QSqlQuery，但在查重失败分支 return 时未 delete -> 内存泄漏
    QSqlQuery *query = new QSqlQuery(db);

    QString checkSql = QString("SELECT id FROM goods WHERE id=%1").arg(_id);
    if (!query->exec(checkSql)) {
        QMessageBox::critical(this, "错误", "数据库查询失败");
        delete query;
        return;
    }
    if (query->next()) {
        QMessageBox::warning(this, "错误", "编号已存在！");
        return;  // 漏洞：query 没有被 delete
    }

    QString insertSql = QString(
                            "INSERT INTO goods(id, name, price, number, description, seller) "
                            "VALUES(%1, '%2', %3, %4, '%5', '%6')"
                            ).arg(_id).arg(_name).arg(_price).arg(_number).arg(_notice).arg(extern_username);

    if (query->exec(insertSql)) {
        QMessageBox::information(this, "成功", "商品发布成功！");

        // 漏洞4：delete 在 show 之后导致窗口被销毁 -> UAF
        MainWindow *mainwindow = new MainWindow(extern_commodity);
        mainwindow->show();
        delete mainwindow;

        delete query;
        this->close();
    } else {
        QMessageBox::critical(this, "失败", "数据库写入错误: " + query->lastError().text());
        delete query;
    }
}

void ReleaseGoodsDialog::on_cancelButton_clicked()
{
    if (extern_from == "admin") {
        MainWindow *mainwindow = new MainWindow(extern_commodity);
        mainwindow->show();
    } else {
        MenuWidget *menu = new MenuWidget;
        menu->show();
    }
    this->close();
}




