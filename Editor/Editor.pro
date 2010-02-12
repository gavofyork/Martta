# #####################################################################
# Automatically generated by qmake (2.01a) Tue Jul 14 15:57:37 2009
# #####################################################################
include(../global.pri)
CONFIG *= qt
DEPS = Language WebView
QT += svg \
	webkit \
	xml
TEMPLATE = app
BASE = ../Language
TWD = $$PWD
TARGET = Editor
include(../Language/dep.pri)
OBJECTS_DIR = build
UI_DIR = build
RCC_DIR = build
MOC_DIR = build
DEPENDS =
QMAKE_LIBDIR += ../support \
	../plugins
INCLUDEPATH *= ../support
DEPENDPATH += .
win32:DESTDIR = .\.

win32:DLLs.commands = @echo Copying libraries...\
	&& copy ..\\support\\support.dll $$DESTDIR \
	&& copy ..\\plugins\\Language.dll $$DESTDIR \
	&& copy ..\\plugins\\WebView.dll $$DESTDIR \
	&& copy ..\\plugins\\Concept.dll $$DESTDIR
QMAKE_EXTRA_TARGETS += DLLs
PRE_TARGETDEPS += DLLs

linux:QMAKE_LFLAGS += -Wl,-rpath,../plugins -Wl,-rpath,../support -Wl,-rpath,../share/martta/support -Wl,-rpath,../share/martta/plugins
macx:QMAKE_LFLAGS += -Wl,-macosx_version_min,10.5 -Wl,-rpath,$$PWD/../plugins -Wl,-rpath,$$PWD/../support
LIBS += -lsupport
TARGET = Martta
DISTFILES += ../TODO
RESOURCES += CodeView.qrc
FORMS += MainWindow.ui
macx:ICON = Martta.icns
include(files.pro)
QMAKE_BUNDLE_DATA += SUPPORT
SUPPORT.version = Versions
SUPPORT.files = $$SUPPORT_HEADERS \
	$$SUPPORT_INLINES
SUPPORT.path = Support
win32: SUPPORT.files += ../support/support.dll ../support/support.lib
linux: SUPPORT.files += ../support/libsupport.so
macx: SUPPORT.files += ../support/libsupport.dylib

INSTALLS = target dotdesktop icon
target.path = $$PREFIX/bin

system(cat Martta.desktop > $${OBJECTS_DIR}/Martta.desktop)
system(echo Exec=$${PREFIX}/bin/Martta >> $${OBJECTS_DIR}/Martta.desktop)
system(echo Path=$${PREFIX}/bin >> $${OBJECTS_DIR}/Martta.desktop)

dotdesktop.files = $${OBJECTS_DIR}/Martta.desktop
dotdesktop.path = $$PREFIX/share/applications
icon.files = Martta.xpm Martta.png
icon.path = $$PREFIX/share/pixmaps

# Input
HEADERS += CodeView.h \
	MainWindow.h \
	Timer.h
SOURCES += CodeView.cpp \
	MainWindow.cpp \
	Timer.cpp \
	main.cpp
OTHER_FILES += Support.js
