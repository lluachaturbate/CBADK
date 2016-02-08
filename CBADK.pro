TEMPLATE = app

QT += qml quick widgets
CONFIG += c++11

SOURCES += main.cpp \
    cbds.cpp \
    cbobject.cpp \
    chatmessage.cpp \
    chatmodel.cpp \
    user.cpp \
    users.cpp

RESOURCES += qml.qrc


HEADERS += \
    cbds.h \
    cbobject.h \
    chatmessage.h \
    chatmodel.h \
    user.h \
    users.h

