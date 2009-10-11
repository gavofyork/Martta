include(../martta.prf)
include(Language.pri)
HEADERS +=  ./Module.h ./Program.h
unix:HEADERS +=  ./Module.cpp ./Program.cpp
SOURCES += .Language-composed.cpp
DEFINES += M_API_Language=M_INAPI

