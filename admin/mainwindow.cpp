#include "mainwindow.h"
#include "releasegoodsdialog.hpp"
#include "chosetabledialog.hpp" // 假设这是上一级选择表的界面

// 全局变量定义
QString extern_commodity;

MainWindow::MainWindow(const QString &commodityTable, QWidget *parent)
    : QMainWindow(parent)
{
    extern_commodity = commodityTable; // 保存当前操作的表名

    // 设置窗口基本属性
    this->setGeometry(100, 100, 1000, 600);
    if(extern_commodity == "goods") this->setWindowTitle("商品管理系统 - 管理员模式");
    else if(extern_commodity == "users") this->setWindowTitle("用户管理系统 - 管理员模式");
    else if(extern_commodity == "orders") this->setWindowTitle("订单管理系统 - 管理员模式");

    // 创建数据模型
    comModel = new QSqlTableModel(this);
    comModel->setTable(extern_commodity);
    comModel->setEditStrategy(QSqlTableModel::OnManualSubmit); // 手动提交策略
    comModel->select(); // 查询数据

    // 创建UI分组
    QGroupBox *tableGroup = createComGroup();
    QGroupBox *detailGroup = createDetailGroup();

    // 布局设置
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(tableGroup, 2); // 左侧占2份
    mainLayout->addWidget(detailGroup, 1); // 右侧占1份

    centralWidget->setLayout(mainLayout);
    this->setCentralWidget(centralWidget);
}

MainWindow::~MainWindow()
{
    // Qt的对象树机制会自动清理子对象
}

QGroupBox *MainWindow::createComGroup()
{
    QGroupBox *box = new QGroupBox("数据列表");

    comView = new QTableView;
    comView->setModel(comModel);
    comView->setSelectionBehavior(QAbstractItemView::SelectRows); // 选中整行
    comView->setSelectionMode(QAbstractItemView::SingleSelection);
    comView->setAlternatingRowColors(true);
    comView->resizeColumnsToContents();

    // 连接点击信号到槽函数
    connect(comView, SIGNAL(clicked(QModelIndex)), this, SLOT(changeComView(QModelIndex)));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(comView);
    box->setLayout(layout);

    return box;
}

QGroupBox *MainWindow::createDetailGroup()
{
    QGroupBox *box = new QGroupBox("详细信息与操作");

    showList = new QListWidget;
    seLineEdit = new QLineEdit;
    seLineEdit->setPlaceholderText("输入名称进行搜索...");

    // 初始化按钮
    deleteBtn = new QPushButton("删除选中");
    addBtn = new QPushButton("添加记录");
    searchBtn = new QPushButton("查找");
    returnBtn = new QPushButton("显示全部");
    quitBtn = new QPushButton("返回上级");

    // 连接信号槽
    connect(deleteBtn, SIGNAL(clicked()), this, SLOT(removeComFromComModel()));
    connect(addBtn, SIGNAL(clicked()), this, SLOT(addNewSlot()));
    connect(searchBtn, SIGNAL(clicked()), this, SLOT(searchSlot()));
    connect(returnBtn, SIGNAL(clicked()), this, SLOT(returnSlot()));
    connect(quitBtn, SIGNAL(clicked()), this, SLOT(quitBtnSlot()));

    // 如果不是操作商品表，可以禁用某些特定的添加功能，视需求而定
    // 这里保持通用逻辑

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(showList, 0, 0, 1, 2); // 列表占满上方

    layout->addWidget(seLineEdit, 1, 0);
    layout->addWidget(searchBtn, 1, 1);

    layout->addWidget(addBtn, 2, 0);
    layout->addWidget(deleteBtn, 2, 1);

    layout->addWidget(returnBtn, 3, 0);
    layout->addWidget(quitBtn, 3, 1);

    box->setLayout(layout);
    return box;
}

void MainWindow::changeComView(QModelIndex index)
{
    showList->clear();

    // 获取当前行记录
    QSqlRecord record = comModel->record(index.row());
    int count = record.count();

    // 遍历所有字段显示详情
    for(int i = 0; i < count; i++)
    {
        QString fieldName = record.fieldName(i);
        QString value = record.value(i).toString();

        QString displayText = QString("%1: %2").arg(fieldName).arg(value);

        item = new QListWidgetItem(showList);
        item->setText(displayText);
        showList->addItem(item);
    }
}

void MainWindow::removeComFromComModel()
{
    int curRow = comView->currentIndex().row();
    if(curRow < 0) {
        QMessageBox::warning(this, "提示", "请先选择要删除的行");
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认删除", "确定要删除该记录吗?",
                                  QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        comModel->removeRow(curRow);

        // 因为设置了OnManualSubmit，需要手动提交
        if(comModel->submitAll()){
            comModel->select(); // 刷新显示
            showList->clear();
            QMessageBox::information(this, "成功", "删除成功");
        } else {
            QMessageBox::critical(this, "失败", "删除失败: " + comModel->lastError().text());
        }
    }
}

void MainWindow::searchSlot()
{
    QString name = seLineEdit->text();
    if(name.isEmpty()){
        QMessageBox::warning(this, "提示", "请输入搜索关键词");
        return;
    }

    // 根据表不同，可能字段名不同。假设都有'name'或者'名称'字段
    // 这里做一个简单的适配，或者默认搜索 "名称"
    QString filterStr;
    if(extern_commodity == "goods") {
        filterStr = QString("名称 LIKE '%%1%'").arg(name);
    } else if (extern_commodity == "users") {
        filterStr = QString("username LIKE '%%1%'").arg(name);
    } else {
        // 默认尝试搜索 name 字段
        filterStr = QString("name LIKE '%%1%'").arg(name);
    }

    comModel->setFilter(filterStr);
    comModel->select();

    if(comModel->rowCount() == 0) {
        QMessageBox::information(this, "提示", "未找到相关记录");
    }
}

void MainWindow::returnSlot()
{
    seLineEdit->clear();
    comModel->setTable(extern_commodity); // 重置表
    comModel->setSort(0, Qt::AscendingOrder);
    comModel->setFilter(""); // 清空过滤器
    comModel->select();
}

void MainWindow::addNewSlot()
{
    // 仅当管理商品时调用 ReleaseGoodsDialog
    if(extern_commodity == "goods") {
        ReleaseGoodsDialog *release = new ReleaseGoodsDialog("admin");
        release->show();
        this->close(); // 关闭当前主窗口，发布完成后由Dialog负责重新打开
    } else {
        QMessageBox::information(this, "提示", "当前模块暂不支持直接添加，请使用注册或下单功能。");
    }
}

void MainWindow::quitBtnSlot()
{
    // 返回上一级选择界面
    choseTableDialog *choose = new choseTableDialog;
    choose->show();
    this->close();
}
