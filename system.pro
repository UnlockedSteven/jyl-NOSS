# -------------------------------------------------
# Project: ShopSystem
# Refactored Directory Structure
# -------------------------------------------------

QT       += core gui sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ShopSystem
TEMPLATE = app

# 【关键】让编译器能找到子文件夹里的头文件
INCLUDEPATH += $$PWD/auth \
               $$PWD/market \
               $$PWD/user \
               $$PWD/admin

# --- Entry Point ---
SOURCES += main.cpp

# --- Auth Module (认证) ---
HEADERS += auth/logindialog.hpp \
           auth/registerdialog.hpp \
           auth/forgetpasswddialog.hpp \
           auth/chosetabledialog.hpp

SOURCES += auth/logindialog.cpp \
           auth/registerdialog.cpp \
           auth/forgetpasswddialog.cpp \
           auth/chosetabledialog.cpp

FORMS   += auth/logindialog.ui \
           auth/registerdialog.ui \
           auth/forgetpasswddialog.ui \
           auth/chosetabledialog.ui

# --- Market Module (商城) ---
HEADERS += market/menuwidget.hpp \
           market/buygoodsdialog.hpp \
           market/releasegoodsdialog.hpp

SOURCES += market/menuwidget.cpp \
           market/buygoodsdialog.cpp \
           market/releasegoodsdialog.cpp

FORMS   += market/menuwidget.ui \
           market/buygoodsdialog.ui \
           market/releasegoodsdialog.ui

# --- User Module (用户) ---
HEADERS += user/myprofiledialog.hpp \
           user/myordersdialog.hpp

SOURCES += user/myprofiledialog.cpp \
           user/myordersdialog.cpp

FORMS   += user/myordersdialog.ui \
           user/myprofiledialog.ui

# --- Admin Module (后台) ---
HEADERS += admin/mainwindow.h
SOURCES += admin/mainwindow.cpp
#FORMS   += admin/mainwindow.ui

# --- Resources ---
# 有资源文件，解开下面注释并确位置正确
# RESOURCES += resources.qrc
