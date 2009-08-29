include(../martta.prf)
include(Types.pri)
HEADERS +=  Interfaces/TypeDefinition.h ./HashType.h ./ListType.h ./StringType.h ./ExplicitType.h ./Array.h ./BuiltinType.h ./BuiltinDeclaration.h ./BuiltinMethod.h ./BuiltinOperator.h ./MemberTemplateType.h ./Memberify.h ./AddressType.h ./Pointer.h ./UndefinedArray.h ./FunctionType.h ./Subscriptable.h Support/SubscriptableRegistrar.h
unix:HEADERS +=  Interfaces/TypeDefinition.cpp ./HashType.cpp ./ListType.cpp ./StringType.cpp ./ExplicitType.cpp ./Array.cpp ./BuiltinType.cpp ./BuiltinDeclaration.cpp ./BuiltinMethod.cpp ./BuiltinOperator.cpp ./MemberTemplateType.cpp ./Memberify.cpp ./AddressType.cpp ./Pointer.cpp ./UndefinedArray.cpp ./FunctionType.cpp ./Subscriptable.cpp Support/SubscriptableRegistrar.cpp
SOURCES += .Types-composed.cpp
DEFINES += M_API_Types=M_INAPI

