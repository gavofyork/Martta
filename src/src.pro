include(files.pro)
CONFIG -= release debug stl

# SET THE CONFIG HERE!
CONFIG += debug warn_on thread qt

profile {
DEFINES += PROFILE
CONFIG += release
}
debug:DEFINES += DEBUG
release:DEFINES += RELEASE

mac:QMAKE_LFLAGS += -Wl,-Sp
unix {
	debug {
		message("Debug build")
		QMAKE_CXXFLAGS_DEBUG += -O0 -g3
	}
	profile {
		message("Profile build")
		QMAKE_CXXFLAGS_RELEASE += -Os -g3
	}
	!profile:release {
		message("Release build")
		QMAKE_CXXFLAGS_RELEASE += -Os -g0
	}
}

#unix:MOC_DIR = /tmp
#unix:RCC_DIR = /tmp
#unix:UI_DIR = /tmp
SUPPORT.version = Versions
SUPPORT.files = $$SUPPORT_HEADERS $$SUPPORT_INLINES
SUPPORT.path = Support

ICON = Martta.icns

QMAKE_BUNDLE_DATA += SUPPORT
TEMPLATE = app
TARGET = ../bin/Martta

QT += xml svg
FORMS += Gui/MainWindow.ui
win32 {
	SUPPORT.files += ../support/support.lib
	LIBS += ../support/Debug/support.lib
}
unix {
     SUPPORT.files += ../support/libsupport.a
	 LIBS += ../support/libsupport.a
}
INCLUDEPATH += ../support $$DIRS
#PRECOMPILED_HEADER = Precompiled.h
DISTFILES += ../TODO
RESOURCES += Operators.qrc
SOURCES = $$MARTTA_COMPOSED
HEADERS += $$MARTTA_HEADERS $$MARTTA_INLINES $$MARTTA_SOURCES
