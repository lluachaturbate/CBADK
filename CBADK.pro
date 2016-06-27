QT       += core gui qml quick quickwidgets script scripttools
CONFIG  += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CBADK
TEMPLATE = app


SOURCES += main.cpp\
        gui.cpp \
    viewer.cpp \
    chatmodel.cpp \
    viewermodel.cpp \
    cbobjectimpl.cpp \
    cbds.cpp

HEADERS  += gui.h \
    chatline.h \
    viewer.h \
    chatmodel.h \
    viewermodel.h \
    cbobjectbase.h \
    cbobjectimpl.h \
    cbjsobject.h \
    cbds.h \
    viewerchatmodel.h \
    settingsdialog.h

RESOURCES += \
    qml.qrc

win32: RC_ICONS = CBADK.ico
