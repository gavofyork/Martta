include(../martta.prf)
include(Projects.pri)
CONFIG += qt
QT = core xml
SOURCES +=  CProject.cpp CSolution.cpp CDependency.cpp MainFunction.cpp Support/DeclarationsHandler.cpp Support/GccXML.cpp
HEADERS +=  CProject.h CSolution.h CDependency.h MainFunction.h Support/DeclarationsHandler.h Support/GccXML.h
