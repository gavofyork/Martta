include(../martta.prf)
include(CTypes.pri)
HEADERS +=  ./CTypes.h
unix:HEADERS += 
SOURCES += .CTypes-composed.cpp
DEFINES += M_API_CTypes=M_INAPI

