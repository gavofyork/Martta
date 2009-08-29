include(../martta.prf)
include(SimpleTypes.pri)
HEADERS +=  ./Const.h ./Reference.h ./PhysicalType.h
unix:HEADERS +=  ./Const.cpp ./Reference.cpp ./PhysicalType.cpp
SOURCES += .SimpleTypes-composed.cpp
DEFINES += M_API_SimpleTypes=M_INAPI

