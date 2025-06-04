QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    add_arenda.cpp \
    add_io.cpp \
    add_prtkl.cpp \
    add_si.cpp \
    add_type_si.cpp \
    add_vo.cpp \
    database.cpp \
    dates.cpp \
    file_menu.cpp \
    forms.cpp \
    key_dialog.cpp \
    main.cpp \
    mainwindow.cpp \
    staff.cpp

HEADERS += \
    add_arenda.h \
    add_io.h \
    add_prtkl.h \
    add_si.h \
    add_type_si.h \
    add_vo.h \
    database.h \
    dates.h \
    file_menu.h \
    forms.h \
    key_dialog.h \
    mainwindow.h \
    staff.h

FORMS += \
    add_arenda.ui \
    add_io.ui \
    add_prtkl.ui \
    add_si.ui \
    add_type_si.ui \
    add_vo.ui \
    dates.ui \
    file_menu.ui \
    forms.ui \
    key_dialog.ui \
    mainwindow.ui \
    staff.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
