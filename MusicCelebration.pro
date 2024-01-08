#-------------------------------------------------
#
# Project created by QtCreator 2017-04-11T15:11:42
#
#-------------------------------------------------

#QT += core gui webenginewidgets

QT += core gui network
QT -= qml quick
CONFIG += c++11
CONFIG -= app_bundle
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MusicCelebration
TEMPLATE = app

# The following define makes your compiler emit  warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
######## MDK SDK ##########
MDK_SDK = $$PWD/qmdk/mdk-sdk
include($$MDK_SDK/mdk.pri)

CONFIG(debug, debug|release) {
    LIBS += -L$$OUT_PWD/qmdk/debug
} else {
    LIBS += -L$$OUT_PWD/qmdk/release
}
android: LIB_SUFFIX=_$$ANDROID_TARGET_ARCH
LIBS += -L$$OUT_PWD/qmdk -lqtmdk$$LIB_SUFFIX

SOURCES += main.cpp\
    lineeditmx.cpp \
        musicshow.cpp \
    singleapplication.cpp \
    dialogmx.cpp \
    buttonmx.cpp \
    blinkbtn.cpp \
    systemtraymx.cpp \
    speedcontrol.cpp \
    mylyric.cpp \
    webview.cpp

HEADERS  += musicshow.h \
    lineeditmx.h \
    singleapplication.h \
    widgetObjName.h \
    dialogmx.h \
    buttonmx.h \
    blinkbtn.h \
    systemtraymx.h \
    speedcontrol.h \
    mylyric.h \
    webview.h

RESOURCES += \
    stylefile.qrc

RC_FILE = iconExe.rc

DISTFILES +=


