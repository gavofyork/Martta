# #####################################################################
# Automatically generated by qmake (2.01a) Wed Jun 4 12:02:05 2008
# #####################################################################
TEMPLATE = lib
CONFIG -= release \
    debug \
    stl \
    qt

win32:QMAKE_CXXFLAGS += /wd4800

# SET THE CONFIG HERE!
CONFIG += debug \
    warn_on \
    thread \
    sharedlib
profile { 
    DEFINES += PROFILE
    CONFIG += release
}
debug:DEFINES += DEBUG
release:DEFINES += RELEASE
unix:DEFINES += M_UNIX
mac:DEFINES += M_MAC
win32:DEFINES += M_WIN
!mac:unix:DEFINES += M_LINUX
mac:QMAKE_LFLAGS += -Wl,-Sp
mac:QMAKE_CXXFLAGS_DEBUG += -fasm-blocks
unix { 
    debug { 
        message("Debug build")
        QMAKE_CXXFLAGS_DEBUG += -O0 \
            -g3
    }
    profile { 
        message("Profile build")
        QMAKE_CXXFLAGS_RELEASE += -Os \
            -g3
    }
    !profile:release { 
        message("Release build")
        QMAKE_CXXFLAGS_RELEASE += -Os \
            -g0
    }
}
DEFINES += M_API_support=M_INAPI
TARGET = support
DEPENDPATH += .
INCLUDEPATH += .
OBJECTS_DIR = build
win32:DESTDIR = ./.

# Input
HEADERS += msSupport.h \
    msList.h \
    msString.h \
    msList.inl \
    msHash.h \
    msHash.inl \
    msRgb.h \
    msTextStream.h \
    msDebug.h \
    msStringList.h \
    msFlags.h
SOURCES += msSupport.cpp \
    msString.cpp \
    msRgb.cpp \
    msTextStream.cpp \
    msDebug.cpp \
    msStringList.cpp \
    msFlags.cpp
