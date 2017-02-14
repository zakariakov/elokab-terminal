#-------------------------------------------------
#
# Project created by QtCreator 2015-05-30T21:58:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = elokab-terminal
TEMPLATE = app
#---------
DESTDIR = ../usr/bin
VERSION		= 0.1
CONFIG += qt \
          release
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
INCLUDEPATH +=build
#-------


LIBS += $(SUBLIBS)  -L../usr/lib -L$$PWD/../usr/lib  -leterminal
QMAKE_INCDIR += $$PWD/../library/include $$PWD/../library

SOURCES += main.cpp\
           mainwindow.cpp \
           settingdialog.cpp \
    buttonColor/buttoncolor.cpp

HEADERS  += mainwindow.h \
            settingdialog.h \
    buttonColor/buttoncolor.h

FORMS    += mainwindow.ui \
            settingdialog.ui



RESOURCES += \
    icons.qrc

TRANSLATIONS    =../usr/share/elokab/translations/ar/elokab-terminal.ts\
                 ../usr/share/elokab/translations/fr/elokab-terminal.ts\
                 ../usr/share/elokab/translations/en/elokab-terminal.ts\

linux-g++*: {
             # Provide relative path from application to elokab librarys
             # ex:app=usr/bin/elokab-applications  library=usr/lib/libelokabmimicon.so
             QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/../lib
             }

#INSTALL
 target.path = /usr/bin
 INSTALLS +=   target
