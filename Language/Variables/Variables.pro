include(../martta.prf)
include(Variables.pri)
SOURCES += ./AssignedVariable.cpp \
	./DefaultConstructedVariable.cpp \
	Interfaces/VariableNamer.cpp \
	Support/VariablePlacer.cpp
HEADERS += ./AssignedVariable.h \
	./DefaultConstructedVariable.h \
	Interfaces/VariableNamer.h \
	Support/VariablePlacer.h
