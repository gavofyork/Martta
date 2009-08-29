include(../martta.prf)
include(Class.pri)
HEADERS +=  ./Class.h Interfaces/Artificial.h ./ArtificialAssignmentOperator.h ./ArtificialCopyConstructor.h ./ArtificialDefaultConstructor.h ./Base.h ./VirtualBase.h ./Virtual.h ./VirtualMethod.h ./VirtualOverload.h ./VirtualPure.h ./MemberReferenced.h
unix:HEADERS +=  ./Class.cpp Interfaces/Artificial.cpp ./ArtificialAssignmentOperator.cpp ./ArtificialCopyConstructor.cpp ./ArtificialDefaultConstructor.cpp ./Base.cpp ./VirtualBase.cpp ./Virtual.cpp ./VirtualMethod.cpp ./VirtualOverload.cpp ./VirtualPure.cpp ./MemberReferenced.cpp
SOURCES += .Class-composed.cpp
DEFINES += M_API_Class=M_INAPI

