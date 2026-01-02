#include <gtest/gtest.h>
//#include <gmock/gmock-matchers.h>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QLineEdit>
#include <QPushButton>
#include <QTimer>
#include <QApplication>
#include <QDir>
#include <QFile>

#include "buygoodsdialog.hpp"

using namespace testing;

extern int extern_id;
extern QString extern_username;

class BuyGoodsTest : public ::testing::Test {
protected:

    QString dbPath;

    void SetUp() override {
        // 1. 计算绝对路径
        dbPath = QCoreApplication::applicationDirPath() + "/database.db";
        qDebug() << "[TestInfo] Target DB Path:" << dbPath;

        // 2. 强制设置当前工作目录，确保 buyGoodsDialog 如果用相对路径也能找到这个文件
        QDir::setCurrent(QCoreApplication::applicationDirPath());

        extern_id = 1;
        extern_username = "UnitTester";

        // 3. 清理环境：先移除连接，再删除物理文件
        {
            QSqlDatabase db = QSqlDatabase::database();
            if(db.isOpen()) db.close();
        }
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);

        // 删除旧文件，确保测试从零开始
        if (QFile::exists(dbPath)) {
            QFile::remove(dbPath);
        }

        // 4. 创建新库并初始化数据
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(dbPath); // 使用绝对路径

        if (db.open()) {
            QSqlQuery q;


            bool b1 = q.exec("CREATE TABLE goods(编号 INTEGER PRIMARY KEY, 名称 TEXT, 价格 DOUBLE, 数量 INT, 备注 TEXT)");
            bool b2 = q.exec("CREATE TABLE orders(id INTEGER PRIMARY KEY, good_id INT, username TEXT, good_name TEXT, good_price DOUBLE, good_number INT, user_notice TEXT)");

            if (!b1 || !b2) qDebug() << "[TestError] Create Table Failed:" << q.lastError().text();

            // 插入数据
            bool b3 = q.exec("INSERT INTO goods(编号, 名称, 价格, 数量, 备注) VALUES(1, 'Apple', 10.0, 100, 'Test Note')");
            if (!b3) qDebug() << "[TestError] Insert Failed:" << q.lastError().text();


            db.close();
        } else {
            qDebug() << "[TestError] SetUp Open Failed:" << db.lastError().text();
        }

        // 移除连接名，给 buyGoodsDialog 腾位置
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    }

    void TearDown() override {
        // 清理连接
        {
            QSqlDatabase db = QSqlDatabase::database();
            if(db.isOpen()) db.close();
        }
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    }

    // 自动关闭弹窗的辅助函数
    void prepareAutoCloseModal() {
        QTimer::singleShot(500, []() { // 稍微延长时间到 500ms，确保弹窗已弹出
            QWidget *widget = QApplication::activeModalWidget();
            if (widget) {
                qDebug() << "[TestInfo] Closing modal widget:" << widget->metaObject()->className();
                widget->close();
            }
        });
    }

    // 验证函数
    int getCurrentStock(int goodId) {
        // 获取当前连接（这应该是 buyGoodsDialog 打开的那个连接）
        QSqlDatabase db = QSqlDatabase::database();

        // 如果业务代码关了连接，或者没开，我们尝试自己打开那个绝对路径的文件
        if (!db.isOpen()) {
            db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName(dbPath); // 再次使用绝对路径
            if(!db.open()) {
                qDebug() << "[TestError] Validation Open Failed";
                return -1;
            }
        }

        QSqlQuery q(db);
        q.prepare("SELECT 数量 FROM goods WHERE 编号 = ?");
        q.addBindValue(goodId);

        if (!q.exec()) {
            qDebug() << "[TestError] Query Failed:" << q.lastError().text();
            return -1;
        }

        if(q.next()) {
            return q.value(0).toInt();
        } else {
            qDebug() << "[TestError] ID" << goodId << "not found in DB:" << db.databaseName();
            return -1;
        }
    }
};

// --- 测试用例 ---

TEST_F(BuyGoodsTest, PurchaseSuccess) {
    buyGoodsDialog dialog;



    QLineEdit *numEdit = dialog.findChild<QLineEdit*>("numberLineEdit");
    QPushButton *btn = dialog.findChild<QPushButton*>("pushButton");

    if (!numEdit || !btn) {
        // 如果找不到控件，打印一下 dialog 的所有子控件，方便调试
        qDebug() << "[TestError] Controls not found! Children list:";
        for(auto c : dialog.children()) qDebug() << " - " << c->objectName() << c->metaObject()->className();
        FAIL() << "UI controls not found";
    }

    numEdit->setText("5");
    prepareAutoCloseModal();
    btn->click();

    EXPECT_EQ(getCurrentStock(1), 95);
}

TEST_F(BuyGoodsTest, PurchaseFail_OverStock) {
    buyGoodsDialog dialog;

    QLineEdit *numEdit = dialog.findChild<QLineEdit*>("numberLineEdit");
    QPushButton *btn = dialog.findChild<QPushButton*>("pushButton");

    if(!numEdit || !btn) FAIL() << "UI controls not found";

    numEdit->setText("101"); // 库存只有100
    prepareAutoCloseModal();
    btn->click();

    EXPECT_EQ(getCurrentStock(1), 100);
}

TEST_F(BuyGoodsTest, PurchaseFail_InvalidInput) {
    buyGoodsDialog dialog;

    QLineEdit *numEdit = dialog.findChild<QLineEdit*>("numberLineEdit");
    QPushButton *btn = dialog.findChild<QPushButton*>("pushButton");

    if(!numEdit || !btn) FAIL() << "UI controls not found";

    // 0
    numEdit->setText("0");
    prepareAutoCloseModal();
    btn->click();
    EXPECT_EQ(getCurrentStock(1), 100);

    // 负数
    numEdit->setText("-10");
    prepareAutoCloseModal();
    btn->click();
    EXPECT_EQ(getCurrentStock(1), 100);
}

TEST_F(BuyGoodsTest, UiInitializationCheck) {
    buyGoodsDialog dialog;

    SUCCEED();
}




