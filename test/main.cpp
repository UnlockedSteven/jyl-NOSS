#include <gtest/gtest.h>
#include <QApplication>
#include <QString>

// =========================================================
// 补全全局变量定义
// =========================================================
// 这些变量在原项目的 main.cpp 中定义。
// 在测试项目中，我们在这里定义它们，以满足链接器的要求。
int extern_id = 0;
QString extern_username = "TestUser";
QString extern_address = "TestAddress";
QString extern_phone = "13800000000";
QString extern_commodity = "TestGood";
QString extern_from = "user";

int main(int argc, char *argv[])
{
    // 初始化 Qt 应用程序对象 (必须在 GUI 测试之前)
    QApplication a(argc, argv);

    // 初始化 Google Test
    ::testing::InitGoogleTest(&argc, argv);

    // 运行所有测试用例
    return RUN_ALL_TESTS();
}
