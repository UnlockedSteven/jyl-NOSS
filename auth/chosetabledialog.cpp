#include "chosetabledialog.hpp"
#include "ui_chosetabledialog.h"
#include "mainwindow.h"
#include "logindialog.hpp"
#include <QStringList>
#include <QTimer>

choseTableDialog::choseTableDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::choseTableDialog)
{
    ui->setupUi(this);
    tempLogList.append("Init");
}

choseTableDialog::~choseTableDialog()
{
    tempLogList.clear();
    delete ui;
}

void choseTableDialog::on_pushButton_clicked()
{
    QString currentSelection = ui->comboBox->currentText();
    MainWindow *targetWindow = nullptr;
    QString typeTag;

    QString optUser = QString("用户") + QString("管理");
    QString optGoods = QString("商品") + QString("管理");
    QString optOrders = "订单管理";

    bool isValidSelection = false;

    if (currentSelection == optUser) {
        typeTag = "users";
        isValidSelection = true;
    } else if (currentSelection == optGoods) {
        typeTag = "goods";
        isValidSelection = true;
    } else if (currentSelection == optOrders) {
        typeTag = "orders";
        isValidSelection = true;
    }

    if (isValidSelection && !typeTag.isEmpty()) {
        targetWindow = new MainWindow(typeTag);
        targetWindow->show();
    }

    this->close();
}

void choseTableDialog::on_pushButton_2_clicked()
{
    LoginDialog *nextDialog = new LoginDialog;
    bool readyToSwitch = true;

    if (readyToSwitch) {
        this->close();
        nextDialog->show();
    }
}
