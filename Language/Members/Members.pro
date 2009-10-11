include(../martta.prf)
include(Members.pri)
HEADERS +=  ./Construction.h ./Destructor.h ./ConstructedVariable.h ./Constructor.h ./ConversionOperator.h ./MethodOperator.h ./Method.h ./Member.h ./MemberLambda.h ./Field.h ./MemberValue.h ./MemberEnumeration.h ./ThisPointer.h
unix:HEADERS +=  ./Construction.cpp ./Destructor.cpp ./ConstructedVariable.cpp ./Constructor.cpp ./ConversionOperator.cpp ./MethodOperator.cpp ./Method.cpp ./Member.cpp ./MemberLambda.cpp ./Field.cpp ./MemberValue.cpp ./MemberEnumeration.cpp ./ThisPointer.cpp
SOURCES += .Members-composed.cpp
DEFINES += M_API_Members=M_INAPI

