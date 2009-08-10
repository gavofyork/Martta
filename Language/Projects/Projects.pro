include(../martta.prf)
include(Projects.pri)
SOURCES +=  ./CProject.cpp ./CSolution.cpp ./CDependency.cpp ./MainFunction.cpp Support/DeclarationsHandler.cpp Support/GccXml.cpp
HEADERS +=  ./CProject.h ./CSolution.h ./CDependency.h ./MainFunction.h Support/DeclarationsHandler.h Support/GccXml.h
CONFIG += qt
QT = core xml
