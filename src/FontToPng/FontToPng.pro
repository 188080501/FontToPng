QT       += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

CONFIG += c++11

include(./JasonQt/JasonQt.pri)

SOURCES += $$PWD/cpp/main.cpp\
        $$PWD/cpp/MainWindow.cpp

HEADERS  += $$PWD/cpp/MainWindow.h

FORMS    += $$PWD/ui/MainWindow.ui \
    ui/IconExport.ui \
    ui/IconNode.ui

macx{
    ICON = ./Resources/Icon/Icon.icns
}

win32{
    RC_FILE = ./Resources/Icon/Icon.rc
}

RESOURCES += \
    MaterialUI/MaterialUI.qrc \
    Font/Font.qrc
