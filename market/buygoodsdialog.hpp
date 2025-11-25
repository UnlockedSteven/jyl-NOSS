#ifndef BUYGOODSDIALOG_HPP
#define BUYGOODSDIALOG_HPP

#include <QDialog>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDateTime>
#include <QList>
#include <QStringList>

namespace Ui {
class buyGoodsDialog;
}

class buyGoodsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit buyGoodsDialog(QWidget *parent = nullptr);
    ~buyGoodsDialog();

private:
    void createDB();
    void createTable();
    Ui::buyGoodsDialog *ui;
    QSqlDatabase db;

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
};

#endif