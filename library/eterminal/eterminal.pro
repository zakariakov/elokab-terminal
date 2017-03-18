TEMPLATE	= lib
VERSION		= 0.1

CONFIG      +=  plugin release
TARGET      = $$qtLibraryTarget(eterminal)
DESTDIR = ../../usr/lib

#CONFIG		+= qt debug_and_release warn_on build_all staticlib #dll

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build


DEFINES 	+= HAVE_POSIX_OPENPT
#or DEFINES 	+= HAVE_GETPT

HEADERS 	= TerminalCharacterDecoder.h Character.h CharacterColor.h \
		KeyboardTranslator.h \
		ExtendedDefaultTranslator.h \
		Screen.h History.h BlockArray.h konsole_wcwidth.h \
		ScreenWindow.h \
		Emulation.h \
		Vt102Emulation.h TerminalDisplay.h Filter.h LineFont.h \
		Pty.h kpty.h kpty_p.h k3process.h k3processcontroller.h \
		Session.h ShellCommand.h \
		qtermwidget.h

SOURCES 	= TerminalCharacterDecoder.cpp \
		KeyboardTranslator.cpp \
		Screen.cpp History.cpp BlockArray.cpp konsole_wcwidth.cpp \
		ScreenWindow.cpp \
		Emulation.cpp \
		Vt102Emulation.cpp TerminalDisplay.cpp Filter.cpp \
		Pty.cpp kpty.cpp k3process.cpp k3processcontroller.cpp \
		Session.cpp ShellCommand.cpp \
		qtermwidget.cpp




#Install
target.path = /usr/lib
INSTALLS    += target
	

DISTFILES += \
    README
