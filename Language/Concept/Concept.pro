include(../martta.prf)
include(Concept.pri)
HEADERS +=  Support/Auxilliary.h Support/AuxilliaryFace.h Support/AuxilliaryRegistrar.h Support/ChangeMan.h Support/CodeScene.h Support/CompletionDelegate.h Support/CullManager.h Support/Dier.h Support/EditDelegateFace.h Support/EditDelegate.h Support/ConceptSupport.h Support/KeyEvent.h Support/Kind.h Support/Meta.h Support/Position.h Support/SafePointer.h Interfaces/ChildValidifier.h Interfaces/Dependee.h Interfaces/Depender.h Interfaces/Familial.h ./Concept.h Support/SpecialKeys.h
unix:HEADERS +=  Support/AuxilliaryRegistrar.cpp Support/ChangeMan.cpp Support/CodeScene.cpp Support/CompletionDelegate.cpp Support/CullManager.cpp Support/Dier.cpp Support/EditDelegateFace.cpp Support/EditDelegate.cpp Support/KeyEvent.cpp Support/Kind.cpp Support/Meta.cpp Support/Position.cpp Interfaces/ChildValidifier.cpp Interfaces/Dependee.cpp Interfaces/Depender.cpp Interfaces/Familial.cpp ./Concept.cpp Support/SpecialKeys.cpp
SOURCES += .Concept-composed.cpp
DEFINES += M_API_Concept=M_INAPI

