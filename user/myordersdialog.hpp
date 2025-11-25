#ifndef MY_ORDERS_DLG_X_HPP
#define MY_ORDERS_DLG_X_HPP

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlQueryModel>
#include <QAbstractItemModel>
#include <QDate>
#include <QPointer>

namespace Ui {
class myOrdersDialog;
}

class myOrdersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit myOrdersDialog(QWidget *parent = nullptr);
    ~myOrdersDialog();

private:
    void createDB();
    Ui::myOrdersDialog *ui;
    QSqlDatabase db;
    QSqlQueryModel model;
    bool isInitialized;
};

#endif