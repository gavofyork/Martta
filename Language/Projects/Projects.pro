include(../martta.prf)
include(Projects.pri)
SOURCES += .Projects-composed.cpp
HEADERS +=  ./CProject.h ./CSolution.h ./CDependency.h ./MainFunction.h Support/DeclarationsHandler.h Support/GccXml.h  ./CProject.cpp ./CSolution.cpp ./CDependency.cpp ./MainFunction.cpp Support/DeclarationsHandler.cpp Support/GccXml.cpp
CONFIG += qt
QT = core xml
