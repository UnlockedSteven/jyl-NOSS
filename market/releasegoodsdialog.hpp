#ifndef RELEASEGOODSDIALOG_X_HPP
#define RELEASEGOODSDIALOG_X_HPP

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QDebug>
#include <QSqlError>
#include <QValidator>
#include <QDoubleValidator>
#include <QIntValidator>

namespace Ui {
class ReleaseGoodsDialog;
}

class ReleaseGoodsDialog : public QDialog
{
    Q_OBJECT

public:
    // 构造函数需要接收来源字符串 ("admin" 或 "user")
    explicit ReleaseGoodsDialog(const QString &from, QWidget *parent = nullptr);
    ~ReleaseGoodsDialog();

private slots:
    void on_releaseButton_clicked();
    void on_cancelButton_clicked();

signals:
    void order();

private:
    void createDB();

    Ui::ReleaseGoodsDialog *ui;
    QSqlDatabase db;
};

#endif // RELEASEGOODSDIALOG_HPP