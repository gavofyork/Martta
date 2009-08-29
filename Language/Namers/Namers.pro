include(../martta.prf)
include(Namers.pri)
HEADERS +=  ./EnumerationNamer.h ./EnumValue.h ./Argument.h ./LambdaNamer.h
unix:HEADERS +=  ./EnumerationNamer.cpp ./EnumValue.cpp ./Argument.cpp ./LambdaNamer.cpp
SOURCES += .Namers-composed.cpp
DEFINES += M_API_Namers=M_INAPI

