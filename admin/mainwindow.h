#ifndef MAINWINDOW_X_H
#define MAINWINDOW_X_H

#include <QMainWindow>
#include <QModelIndex>
#include <QGroupBox>
#include <QSqlTableModel>
#include <QTableView>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QMessageBox>
#include <QSqlRecord>
#include <QDebug>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // 构造函数接收表名（如 "goods", "users", "orders"）
    explicit MainWindow(const QString &commodityTable, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 槽函数
    void changeComView(QModelIndex index); // 定位选择的行，并在右边list中显示详情
    void removeComFromComModel();          // 删除记录
    void searchSlot();                     // 按名称查找
    void returnSlot();                     // 重置列表/返回
    void addNewSlot();                     // 添加按钮，创建添加记录的对话框
    void quitBtnSlot();                    // 退出/返回上一级

private:
    QGroupBox *createComGroup();           // 创建左侧表格区域
    QGroupBox *createDetailGroup();        // 创建右侧详情及控制区域

    QSqlTableModel *comModel;
    QTableView *comView;
    QListWidget *showList;
    QListWidgetItem *item;

    // 搜索框
    QLineEdit *seLineEdit;

    // 按钮对象
    QPushButton *addBtn;
    QPushButton *deleteBtn;
    QPushButton *searchBtn;
    QPushButton *returnBtn;
    QPushButton *quitBtn;
};

#endif // MAINWINDOW_H
