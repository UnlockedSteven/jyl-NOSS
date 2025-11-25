#ifndef MENUWIDGET_X_HPP
#define MENUWIDGET_X_HPP

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QDebug>
#include <QSqlError>
#include <QValidator>
#include <QPalette>
#include <QBrush>
#include <QList>
#include <QIcon>

QT_BEGIN_NAMESPACE
namespace Ui { class MenuWidget; }
QT_END_NAMESPACE

class MenuWidget : public QWidget
{
    Q_OBJECT

public:
    ~MenuWidget();
    explicit MenuWidget(QWidget *parent = nullptr);

    void createTable();
    void queryTable();
    void createDB();

private slots:
    void on_myOrderButton_clicked();
    void on_buyButton_clicked();
    void on_myinfoButton_clicked();
    void on_releaseButton_clicked();
    void on_searchButton_clicked();
    void on_orderButton_clicked();
    void on_pushButton_clicked();
    void on_RenewpushButton_clicked();

private:
    void initTable();

    Ui::MenuWidget *ui;
    QSqlDatabase db;
    QSqlQueryModel model;
    int refreshCounter;
};

#endif
