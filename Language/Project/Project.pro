include(../martta.prf)
include(Project.pri)
HEADERS +=  ./Project.h ./Solution.h
unix:HEADERS +=  ./Project.cpp ./Solution.cpp
SOURCES += .Project-composed.cpp
DEFINES += M_API_Project=M_INAPI

