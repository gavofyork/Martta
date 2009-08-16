include(../martta.prf)
include(Operator.pri)
HEADERS +=  ./Operator.h Support/OperatorRegistrar.h
unix:HEADERS +=  ./Operator.cpp Support/OperatorRegistrar.cpp
SOURCES += .Operator-composed.cpp

