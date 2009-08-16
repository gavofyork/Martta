include(../martta.prf)
include(CStuff.pri)
HEADERS +=  ./TopLevel.h ./Root.h ./TopLevelType.h ./Function.h ./Variable.h ./Struct.h ./Union.h ./Typedef.h ./Enumeration.h ./Namespace.h
unix:HEADERS +=  ./TopLevel.cpp ./Root.cpp ./TopLevelType.cpp ./Function.cpp ./Variable.cpp ./Struct.cpp ./Union.cpp ./Typedef.cpp ./Enumeration.cpp ./Namespace.cpp
SOURCES += .CStuff-composed.cpp

