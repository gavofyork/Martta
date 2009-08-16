include(../martta.prf)
include(Statement.pri)
HEADERS +=  ./ValueDefiner.h Interfaces/Corporal.h Interfaces/Conditional.h ./Statement.h ./Primary.h ./BareTyped.h ./Typed.h ./Untyped.h ./Compound.h
unix:HEADERS +=  ./ValueDefiner.cpp Interfaces/Corporal.cpp Interfaces/Conditional.cpp ./Statement.cpp ./Primary.cpp ./BareTyped.cpp ./Typed.cpp ./Untyped.cpp ./Compound.cpp
SOURCES += .Statement-composed.cpp

