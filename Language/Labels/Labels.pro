include(../martta.prf)
include(Labels.pri)
HEADERS +=  Interfaces/Labelled.h ./ConstLabel.h ./AccessLabel.h ./Label.h ./IdLabel.h ./TextLabel.h ./OperatorLabel.h
unix:HEADERS +=  Interfaces/Labelled.cpp ./ConstLabel.cpp ./AccessLabel.cpp ./Label.cpp ./IdLabel.cpp ./TextLabel.cpp ./OperatorLabel.cpp
SOURCES += .Labels-composed.cpp
DEFINES += M_API_Labels=M_INAPI

