include(../martta.prf)
include(Comment.pri)
HEADERS +=  ./Comment.h
unix:HEADERS +=  ./Comment.cpp
SOURCES += .Comment-composed.cpp
DEFINES += M_API_Comment=M_INAPI

