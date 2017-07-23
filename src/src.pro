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
    $$PWD/gameengine.h \
    $$PWD/gamematerial.h \
    $$PWD/gamewidget.h \
    $$PWD/gameworld.h \
    $$PWD/globals.h \
    $$PWD/utils.h \
    $$PWD/materialradiobutton.h \
    $$PWD/mainwindow.h

SOURCES += \
    $$PWD/gameengine.cpp \
    $$PWD/gamematerial.cpp \
    $$PWD/gamewidget.cpp \
    $$PWD/gameworld.cpp \
    $$PWD/materialradiobutton.cpp \
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

