include(../martta.prf)
include(Declaration.pri)
HEADERS +=  Support/IdentifierSet.h Support/IdentifierSetRegistrar.h Interfaces/Identifiable.h Support/ModelPtr.h Support/ModelPtrFace.h Support/ModelPtrRegistrar.h Interfaces/Named.h ./NameEntryPoint.h ./Declaration.h
unix:HEADERS +=  Support/IdentifierSet.cpp Support/IdentifierSetRegistrar.cpp Interfaces/Identifiable.cpp Support/ModelPtr.cpp Support/ModelPtrFace.cpp Support/ModelPtrRegistrar.cpp Interfaces/Named.cpp ./NameEntryPoint.cpp ./Declaration.cpp
SOURCES += .Declaration-composed.cpp
DEFINES += M_API_Declaration=M_INAPI

