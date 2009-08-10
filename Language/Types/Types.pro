include(../martta.prf)
include(Types.pri)
SOURCES +=  Interfaces/TypeDefinition.cpp ./ExplicitType.cpp ./HashType.cpp ./ListType.cpp ./StringType.cpp ./Array.cpp ./BuiltinType.cpp ./BuiltinDeclaration.cpp ./BuiltinMethod.cpp ./BuiltinOperator.cpp ./MemberTemplateType.cpp ./Memberify.cpp ./AddressType.cpp ./Pointer.cpp ./UndefinedArray.cpp ./FunctionType.cpp ./Subscriptable.cpp Support/SubscriptableRegistrar.cpp
HEADERS +=  Interfaces/TypeDefinition.h ./ExplicitType.h ./HashType.h ./ListType.h ./StringType.h ./Array.h ./BuiltinType.h ./BuiltinDeclaration.h ./BuiltinMethod.h ./BuiltinOperator.h ./MemberTemplateType.h ./Memberify.h ./AddressType.h ./Pointer.h ./UndefinedArray.h ./FunctionType.h ./Subscriptable.h Support/SubscriptableRegistrar.h

