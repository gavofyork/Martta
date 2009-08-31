include(../martta.prf)
include(CQualifiers.pri)
HEADERS +=  ./CQualifiers.h
unix:HEADERS += 
SOURCES += .CQualifiers-composed.cpp
DEFINES += M_API_CQualifiers=M_INAPI

