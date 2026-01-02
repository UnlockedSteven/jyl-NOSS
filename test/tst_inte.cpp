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

// 业务头文件
#include "buygoodsdialog.hpp"

using namespace testing;

// 模拟外部全局变量
extern int extern_id;
extern QString extern_username;

// =========================================================================
// 基础测试环境类 (Base Fixture)
// 所有测试组都继承自这个类，用于统一管理数据库初始化和销毁
// =========================================================================
class MarketIntegrationBase : public ::testing::Test {
protected:
    QString dbPath;

    void SetUp() override {
        // 1. 设置数据库路径
        // 注意：必须与业务代码中使用的文件名一致（通常是 database.db）
        // 如果业务代码写死 database.db，这里必须也是 database.db
        dbPath = QCoreApplication::applicationDirPath() + "/database.db";
        QDir::setCurrent(QCoreApplication::applicationDirPath());

        // 2. 清理环境：断开所有连接并删除旧文件
        {
            // 获取并关闭默认连接
            QSqlDatabase db = QSqlDatabase::database();
            if(db.isOpen()) db.close();
        }
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);

        // 删除物理文件，确保每次测试都是全新的环境
        if (QFile::exists(dbPath)) {
            QFile::remove(dbPath);
        }

        // 3. 重建数据库结构与初始数据
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(dbPath);

        if (db.open()) {
            QSqlQuery q;
            // 创建表结构
            q.exec("CREATE TABLE goods(编号 INTEGER PRIMARY KEY, 名称 TEXT, 价格 DOUBLE, 数量 INT, 备注 TEXT)");
            q.exec("CREATE TABLE orders(id INTEGER PRIMARY KEY, good_id INT, username TEXT, good_name TEXT, good_price DOUBLE, good_number INT, user_notice TEXT)");

            // 插入初始数据：编号1，苹果，100个
            q.exec("INSERT INTO goods VALUES(1, 'Apple', 10.0, 100, 'Init Note')");

            // 关键：关闭连接。让业务代码自己在运行时重新建立连接，模拟真实启动流程
            db.close();
        }

        // 移除连接定义，避免冲突
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);

        // 4. 初始化默认的用户身份
        extern_id = 1;
        extern_username = "DefaultUser";
    }

    void TearDown() override {
        // 测试结束后清理
        {
            QSqlDatabase db = QSqlDatabase::database();
            if(db.isOpen()) db.close();
        }
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    }

    // 辅助工具：模拟购买操作
    void simulatePurchase(const QString& inputVal) {
        buyGoodsDialog dialog;
        QLineEdit *numEdit = dialog.findChild<QLineEdit*>("numberLineEdit");
        QPushButton *btn = dialog.findChild<QPushButton*>("pushButton");

        if (numEdit && btn) {
            numEdit->setText(inputVal);

            // 使用 Timer 异步关闭模态窗口，防止 test 阻塞
            QTimer::singleShot(100, []() {
                if(auto w = QApplication::activeModalWidget()) w->close();
            });

            btn->click();
        } else {
            qDebug() << "FATAL: UI components not found in buyGoodsDialog!";
        }
    }

    // 辅助工具：读取当前库存
    // 使用独立的连接名 "TestReader"，避免干扰业务逻辑的主连接
    int getCurrentStock() {
        {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "TestReader");
            db.setDatabaseName(dbPath);
            if (!db.open()) return -999;

            QSqlQuery q(db);
            q.exec("SELECT 数量 FROM goods WHERE 编号=1");
            if(q.next()) {
                int val = q.value(0).toInt();
                db.close();
                return val;
            }
            db.close();
        }
        QSqlDatabase::removeDatabase("TestReader");
        return -1;
    }

    // 辅助工具：读取订单总数
    int getOrderCount() {
        int count = 0;
        {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "TestReader");
            db.setDatabaseName(dbPath);
            if (!db.open()) return -999;

            QSqlQuery q(db);
            q.exec("SELECT COUNT(*) FROM orders");
            if(q.next()) count = q.value(0).toInt();
            db.close();
        }
        QSqlDatabase::removeDatabase("TestReader");
        return count;
    }

    // 辅助工具：获取最后一条订单的用户名
    QString getLastOrderUser() {
        QString name = "";
        {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "TestReader");
            db.setDatabaseName(dbPath);
            if (!db.open()) return "DB_ERROR";

            QSqlQuery q(db);
            q.exec("SELECT username FROM orders ORDER BY id DESC LIMIT 1");
            if(q.next()) name = q.value(0).toString();
            db.close();
        }
        QSqlDatabase::removeDatabase("TestReader");
        return name;
    }
};

// =========================================================================
// 第 1 组：交易流程与数据一致性集成 (Transaction Flow Group)
// 目的：集成验证“输入 -> 计算 -> 库存变更 -> 订单生成”这一整条正向链路
// =========================================================================
class Group1_TransactionFlow : public MarketIntegrationBase {};

// 子功能 1: 标准单次购买
TEST_F(Group1_TransactionFlow, StandardSinglePurchase) {
    simulatePurchase("10"); // 买10个

    // 验证点：库存从100变为90，且生成了1个订单
    EXPECT_EQ(getCurrentStock(), 90);
    EXPECT_EQ(getOrderCount(), 1);
}

// 子功能 2: 连续多次购买 (累积效应)
TEST_F(Group1_TransactionFlow, SequentialPurchases) {
    simulatePurchase("10"); // 剩90
    simulatePurchase("20"); // 剩70
    simulatePurchase("5");  // 剩65

    // 验证点：库存正确累积扣除，订单生成3条
    EXPECT_EQ(getCurrentStock(), 65);
    EXPECT_EQ(getOrderCount(), 3);
}

// 子功能 3: 清空库存边界 (买光)
TEST_F(Group1_TransactionFlow, ClearancePurchase) {
    simulatePurchase("100"); // 一次性买光

    // 验证点：库存归零，不为负
    EXPECT_EQ(getCurrentStock(), 0);
    EXPECT_EQ(getOrderCount(), 1);
}

// =========================================================================
// 第 2 组：防御性编程与输入验证集成 (Defense & Validation Group)
// 目的：集成验证“UI输入限制 -> 逻辑拦截 -> 数据库保护”
// =========================================================================
class Group2_InputDefense : public MarketIntegrationBase {};

// 子功能 1: 负数攻击
TEST_F(Group2_InputDefense, NegativeInputAttack) {
    simulatePurchase("-50");

    // 验证点：库存岿然不动，没有订单生成
    EXPECT_EQ(getCurrentStock(), 100);
    EXPECT_EQ(getOrderCount(), 0);
}

// 子功能 2: 零值购买
TEST_F(Group2_InputDefense, ZeroQuantityPurchase) {
    simulatePurchase("0");

    // 验证点：无效交易不应影响数据库
    EXPECT_EQ(getCurrentStock(), 100);
    EXPECT_EQ(getOrderCount(), 0);
}

// 子功能 3: 超卖请求 (超过库存)
TEST_F(Group2_InputDefense, OverStockRequest) {
    simulatePurchase("101"); // 试图买101个 (库存只有100)

    // 验证点：系统拦截，库存保持100，未生成订单
    EXPECT_EQ(getCurrentStock(), 100);
    EXPECT_EQ(getOrderCount(), 0);
}

// 子功能 4: 非法字符输入 (模拟SQL注入或格式错误)
TEST_F(Group2_InputDefense, NonNumericGarbageInput) {
    simulatePurchase("abc#$"); // 乱码

    // 验证点：验证器拦截，数据库不应被修改
    EXPECT_EQ(getCurrentStock(), 100);
    EXPECT_EQ(getOrderCount(), 0);
}

// =========================================================================
// 第 3 组：上下文感知与多用户会话集成 (Context Awareness Group)
// 目的：集成验证“全局状态(用户ID) -> 业务逻辑 -> 订单归属”
// =========================================================================
class Group3_ContextAwareness : public MarketIntegrationBase {};

// 子功能 1: 用户A的购买行为记录
TEST_F(Group3_ContextAwareness, PurchaseAsUserAlpha) {
    // 模拟环境切换
    extern_id = 101;
    extern_username = "User_Alpha";

    simulatePurchase("5");

    // 验证点：订单表里的username字段必须是 User_Alpha
    EXPECT_EQ(getLastOrderUser(), "User_Alpha");
}

// 子功能 2: 用户B的购买行为记录
TEST_F(Group3_ContextAwareness, PurchaseAsUserBeta) {
    // 模拟环境切换
    extern_id = 102;
    extern_username = "User_Beta";

    simulatePurchase("10");

    // 验证点：订单表里的username字段必须是 User_Beta
    EXPECT_EQ(getLastOrderUser(), "User_Beta");
}

// 子功能 3: 会话切换 (A买完，B接着买)
TEST_F(Group3_ContextAwareness, SessionSwitchingSequence) {
    // 1. Alpha 买 10 个
    extern_username = "User_Alpha";
    simulatePurchase("10");

    // 2. Beta 买 20 个
    extern_username = "User_Beta";
    simulatePurchase("20");

    // 验证点：
    // 库存应该是 100 - 10 - 20 = 70
    EXPECT_EQ(getCurrentStock(), 70);

    // 最后一条订单应该是 Beta 的
    EXPECT_EQ(getLastOrderUser(), "User_Beta");

    // 额外验证：数据库应该有两条记录，分别属于不同人
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "TestReader");
        db.setDatabaseName(dbPath);
        db.open();
        QSqlQuery q(db);

        q.exec("SELECT COUNT(*) FROM orders WHERE username='User_Alpha'");
        if(q.next()) EXPECT_EQ(q.value(0).toInt(), 1); // Alpha 有1单

        q.exec("SELECT COUNT(*) FROM orders WHERE username='User_Beta'");
        if(q.next()) EXPECT_EQ(q.value(0).toInt(), 1); // Beta 有1单

        db.close();
    }
    QSqlDatabase::removeDatabase("TestReader");
}


