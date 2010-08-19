include(../martta.prf)
include(Experimental.pri)
HEADERS +=  ./Lambda.h
unix:HEADERS +=  ./Lambda.cpp
SOURCES += .Experimental-composed.cpp
DEFINES += M_API_Experimental=M_INAPI

