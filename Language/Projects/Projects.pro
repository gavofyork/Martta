include(../martta.prf)
include(Projects.pri)
HEADERS +=  ./CProject.h ./CSolution.h ./CDependency.h ./MainFunction.h Support/DeclarationsHandler.h Support/GccXml.h
unix:HEADERS +=  ./CProject.cpp ./CSolution.cpp ./CDependency.cpp ./MainFunction.cpp Support/DeclarationsHandler.cpp Support/GccXml.cpp
SOURCES += .Projects-composed.cpp
DEFINES += M_API_Projects=M_INAPI
CONFIG += qt
QT = core xml
