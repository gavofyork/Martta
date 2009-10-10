include(../martta.prf)
include(TypeConcept.pri)
HEADERS +=  ./TypeConcept.h ./ModifyingType.h Interfaces/TypedOwner.h Interfaces/TypeNamer.h Support/Type.h
unix:HEADERS +=  ./TypeConcept.cpp ./ModifyingType.cpp Interfaces/TypedOwner.cpp Interfaces/TypeNamer.cpp Support/Type.cpp
SOURCES += .TypeConcept-composed.cpp
DEFINES += M_API_TypeConcept=M_INAPI

