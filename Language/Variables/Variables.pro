include(../martta.prf)
include(Variables.pri)
HEADERS +=  Interfaces/VariableNamer.h Support/VariablePlacer.h ./AssignedVariable.h ./DefaultConstructedVariable.h
unix:HEADERS +=  Interfaces/VariableNamer.cpp Support/VariablePlacer.cpp ./AssignedVariable.cpp ./DefaultConstructedVariable.cpp
SOURCES += .Variables-composed.cpp
DEFINES += M_API_Variables=M_INAPI

