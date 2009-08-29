include(../martta.prf)
include(TypeEntity.pri)
HEADERS +=  ./TypeEntity.h ./ModifyingType.h Interfaces/TypedOwner.h Interfaces/TypeNamer.h Support/Type.h
unix:HEADERS +=  ./TypeEntity.cpp ./ModifyingType.cpp Interfaces/TypedOwner.cpp Interfaces/TypeNamer.cpp Support/Type.cpp
SOURCES += .TypeEntity-composed.cpp
DEFINES += M_API_TypeEntity=M_INAPI

