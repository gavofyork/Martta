include(../martta.prf)
include(Variables.pri)
SOURCES += .Variables-composed.cpp
HEADERS +=  Interfaces/VariableNamer.h Support/VariablePlacer.h ./AssignedVariable.h ./DefaultConstructedVariable.h  Interfaces/VariableNamer.cpp Support/VariablePlacer.cpp ./AssignedVariable.cpp ./DefaultConstructedVariable.cpp

