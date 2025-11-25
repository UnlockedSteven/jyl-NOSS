#include "logindialog.hpp"
#include <QSqlDatabase>
#include <QApplication>
#include <QString>



QString extern_username;  // 当前登录用户名
QString extern_phone;     // 当前用户电话
QString extern_address;   // 当前用户地址
int extern_id;            // 当前操作的商品/订单ID



int main(int argc, char *argv[])
{
// 适配高分屏 (High DPI Scaling)
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication a(argc, argv);

    // 启动登录界面
    LoginDialog w;
    w.show();

    return a.exec();
}
