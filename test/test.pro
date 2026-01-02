QT       += core gui sql widgets testlib
CONFIG   += console c++17

TARGET = test_app
TEMPLATE = app

# 1. 禁用 Qt 自动生成的 GTest 配置 (我们手动配置)
# include(gtest_dependency.pri) # <--- 注释掉或删除这一行

# 2. 定义根目录变量，方便引用
PROJECT_ROOT = $$PWD/..

# 3. 包含路径 (让编译器能找到头文件)
INCLUDEPATH += $$PROJECT_ROOT \
               $$PROJECT_ROOT/admin \
               $$PROJECT_ROOT/auth \
               $$PROJECT_ROOT/market \
               $$PROJECT_ROOT/user \
               $$PROJECT_ROOT/googletest/googletest/include \
               $$PROJECT_ROOT/googletest/googletest

# 4. Google Test 核心源码 (直接编译源码，不依赖库文件)
SOURCES += $$PROJECT_ROOT/googletest/googletest/src/gtest-all.cc

# 5. 测试项目本身的入口和测试用例
SOURCES += main.cpp \
           tst_inte.cpp \
           tst_unittest.cpp

# 6. 项目业务逻辑代码 (按模块引入)
# ---------------------------------------------------
# Market 模块
SOURCES += $$PROJECT_ROOT/market/buygoodsdialog.cpp \
           $$PROJECT_ROOT/market/menuwidget.cpp \
           $$PROJECT_ROOT/market/releasegoodsdialog.cpp

HEADERS += $$PROJECT_ROOT/market/buygoodsdialog.hpp \
           $$PROJECT_ROOT/market/menuwidget.hpp \
           $$PROJECT_ROOT/market/releasegoodsdialog.hpp

FORMS   += $$PROJECT_ROOT/market/buygoodsdialog.ui \
           $$PROJECT_ROOT/market/menuwidget.ui \
           $$PROJECT_ROOT/market/releasegoodsdialog.ui

# User 模块
SOURCES += $$PROJECT_ROOT/user/myordersdialog.cpp \
           $$PROJECT_ROOT/user/myprofiledialog.cpp

HEADERS += $$PROJECT_ROOT/user/myordersdialog.hpp \
           $$PROJECT_ROOT/user/myprofiledialog.hpp

FORMS   += $$PROJECT_ROOT/user/myordersdialog.ui \
           $$PROJECT_ROOT/user/myprofiledialog.ui

# Auth 模块
SOURCES += $$PROJECT_ROOT/auth/chosetabledialog.cpp \
           $$PROJECT_ROOT/auth/forgetpasswddialog.cpp \
           $$PROJECT_ROOT/auth/logindialog.cpp \
           $$PROJECT_ROOT/auth/registerdialog.cpp

HEADERS += $$PROJECT_ROOT/auth/chosetabledialog.hpp \
           $$PROJECT_ROOT/auth/forgetpasswddialog.hpp \
           $$PROJECT_ROOT/auth/logindialog.hpp \
           $$PROJECT_ROOT/auth/registerdialog.hpp

FORMS   += $$PROJECT_ROOT/auth/chosetabledialog.ui \
           $$PROJECT_ROOT/auth/forgetpasswddialog.ui \
           $$PROJECT_ROOT/auth/logindialog.ui \
           $$PROJECT_ROOT/auth/registerdialog.ui

# Admin 模块
SOURCES += $$PROJECT_ROOT/admin/mainwindow.cpp
HEADERS += $$PROJECT_ROOT/admin/mainwindow.h

# 注意：我们 **绝对不** 包含 $$PROJECT_ROOT/main.cpp

# 7. 编译参数 (不需要 -fsanitize=address 以避免环境报错)
QMAKE_CXXFLAGS += -g
