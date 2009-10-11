include(../martta.prf)
include(CLanguage.pri)
HEADERS +=  ./CModule.h ./CProgram.h ./CDependency.h ./MainFunction.h Support/DeclarationsHandler.h Support/GccXml.h
unix:HEADERS +=  ./CModule.cpp ./CProgram.cpp ./CDependency.cpp ./MainFunction.cpp Support/DeclarationsHandler.cpp Support/GccXml.cpp
SOURCES += .CLanguage-composed.cpp
DEFINES += M_API_CLanguage=M_INAPI
CONFIG += qt
QT = core xml
