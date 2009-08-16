include(../martta.prf)
include(Entity.pri)
HEADERS +=  Support/Auxilliary.h Support/AuxilliaryFace.h Support/AuxilliaryRegistrar.h Support/ChangeMan.h Support/CodeScene.h Support/CompletionDelegate.h Support/CullManager.h Support/Dier.h Support/EditDelegate.h Support/EntitySupport.h Support/KeyEvent.h Support/Kind.h Support/Meta.h Support/Position.h Support/SafePointer.h Interfaces/ChildValidifier.h Interfaces/Dependee.h Interfaces/Depender.h Interfaces/Familial.h ./Entity.h Support/SpecialKeys.h
unix:HEADERS +=  Support/AuxilliaryRegistrar.cpp Support/ChangeMan.cpp Support/CodeScene.cpp Support/CompletionDelegate.cpp Support/CullManager.cpp Support/Dier.cpp Support/EditDelegate.cpp Support/KeyEvent.cpp Support/Kind.cpp Support/Meta.cpp Support/Position.cpp Interfaces/ChildValidifier.cpp Interfaces/Dependee.cpp Interfaces/Depender.cpp Interfaces/Familial.cpp ./Entity.cpp Support/SpecialKeys.cpp
SOURCES += .Entity-composed.cpp

