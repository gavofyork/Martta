include(../martta.prf)
include(Language.pri)
HEADERS +=  ./Project.h ./Solution.h
unix:HEADERS +=  ./Project.cpp ./Solution.cpp
SOURCES += .Language-composed.cpp
DEFINES += M_API_Language=M_INAPI

