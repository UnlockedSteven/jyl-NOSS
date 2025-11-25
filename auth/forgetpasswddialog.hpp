#ifndef FORGET_PWD_DLG_HPP
#define FORGET_PWD_DLG_HPP

#include <QDialog>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QString>
#include <QSqlRecord>
#include <QVector>
#include <QTime>
#include <QBuffer>

namespace Ui {
class forgetPasswdDialog;
}

class forgetPasswdDialog : public QDialog
{
    Q_OBJECT

public:
    ~forgetPasswdDialog();
    explicit forgetPasswdDialog(QWidget *parent = nullptr);

private slots:
    void on_changeButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::forgetPasswdDialog *ui;
    QSqlDatabase db;
    void createDB();
    int retryCount;
};

#endif
