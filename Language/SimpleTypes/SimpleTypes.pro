include(../martta.prf)
include(SimpleTypes.pri)
HEADERS +=  ./Const.h ./Reference.h ./PhysicalType.h
unix:HEADERS +=  ./Const.cpp ./Reference.cpp ./PhysicalType.cpp
SOURCES += .SimpleTypes-composed.cpp

