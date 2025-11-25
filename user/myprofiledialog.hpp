#ifndef MY_PROFILE_DLG_X_HPP
#define MY_PROFILE_DLG_X_HPP

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QDebug>
#include <QSqlError>
#include <QValidator>
#include <QString>
#include <QTimer>

namespace Ui {
class myProfileDialog;
}

class myProfileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit myProfileDialog(QWidget *parent = nullptr);
    ~myProfileDialog();

private slots:
    void on_changeButton_clicked();

private:
    void createDB();
    void InitInfo(); // 初始化显示信息

    Ui::myProfileDialog *ui;
    QSqlDatabase db;
    bool editModeEnabled;
};

#endif