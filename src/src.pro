#-------------------------------------------------
# ElementDots created by QtCreator
#-------------------------------------------------
TEMPLATE = app
TARGET   = ElementDots
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG  += no_keyword

# QT += c++11
QMAKE_CXXFLAGS += -std=c++11

lessThan(QT_VERSION, 5.0) {
    warning("prefere to build it with Qt 5.0")
}

#-------------------------------------------------
# INCLUDE
#-------------------------------------------------
INCLUDEPATH += $$PWD/../include/


#-------------------------------------------------
# SOURCES
#-------------------------------------------------
HEADERS += \
    $$PWD/about.h \
    $$PWD/builddefs.h \
    $$PWD/gamewidget.h \
    $$PWD/globals.h \
    $$PWD/mainwindow.h

SOURCES += \
    $$PWD/gamewidget.cpp \
    $$PWD/main.cpp \
    $$PWD/mainwindow.cpp

FORMS += \
    $$PWD/mainwindow.ui


#-------------------------------------------------
# RESOURCES
#-------------------------------------------------
RESOURCES = $$PWD/resources.qrc

win32|unix {
    RC_FILE = $$PWD/resources_win.rc
}
