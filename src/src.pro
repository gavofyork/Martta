#unix:MOC_DIR = /tmp
#unix:RCC_DIR = /tmp
#unix:UI_DIR = /tmp
SUPPORT.version = Versions
SUPPORT.files = ../support/msSupport.h ../support/msString.h ../support/msList.h ../support/msList.inl ../support/msHash.h ../support/msHash.inl
SUPPORT.path = Support

ICON = Martta.icns

QMAKE_BUNDLE_DATA += SUPPORT
TEMPLATE = app
CONFIG += warn_on \
	  debug \
	  thread \
          qt
TARGET = ../bin/Martta
mac:QMAKE_LFLAGS += -Wl,-Sp
CONFIG -= stl
QT += xml svg
FORMS += Gui/MainWindow.ui
win32 {
	SUPPORT.files += ../support/support.lib
	LIBS += ../support/Debug/support.lib
}
unix {
     SUPPORT.files += ../support/libsupport.a
	 LIBS += ../support/libsupport.a
}
INCLUDEPATH += ../support
mac:QMAKE_CXXFLAGS_DEBUG += -O0 -g3
PRECOMPILED_HEADER = Precompiled.h
DISTFILES += ../TODO
RESOURCES += Operators.qrc
#INCLUDEPATH += /usr/local/include/boost-1_35
INCLUDEPATH += Gui Language Language/Declarations Language/Labels Language/Operations Language/Statements Language/TypeDefinitions Language/TypedStatements Language/Types Language/ValueDefinitions Main Support
ALL_SOURCES = main.cpp tests.cpp Gui/CodeScene.cpp Gui/MainWindow.cpp Language/Artificial.cpp Language/Base.cpp Language/Declarations/DeclarationEntity.cpp Language/Declarations/NamespaceEntity.cpp Language/Declarations/RootEntity.cpp Language/Declarations/SubAddressable.cpp Language/Entity.cpp Language/Identifiable.cpp Language/Labels/AccessLabel.cpp Language/Labels/ConstLabel.cpp Language/Labels/Label.cpp Language/Labels/OperatorLabel.cpp Language/Labels/TextLabel.cpp Language/Members/Member.cpp Language/Members/MemberEnumeration.cpp Language/Members/MemberVariable.cpp Language/Operations/AssignmentOperation.cpp Language/Operations/BinaryOperation.cpp Language/Operations/DereferenceOperation.cpp Language/Operations/GenericMemberOperation.cpp Language/Operations/LongMemberOperation.cpp Language/Operations/MemberOperation.cpp Language/Operations/NewOperation.cpp Language/Operations/Operation.cpp Language/Operations/ReferenceOperation.cpp Language/Operations/SimpleBinaryOperation.cpp Language/Operations/SimpleUnaryOperation.cpp Language/Operations/SubscriptOperation.cpp Language/Operations/UnaryOperation.cpp Language/Statements/AssignedVariable.cpp Language/Statements/BareTyped.cpp Language/Statements/BreakStatement.cpp Language/Statements/Compound.cpp Language/Statements/ConstructedVariable.cpp Language/Statements/DefaultConstructedVariable.cpp Language/Statements/DeleteOperation.cpp Language/Statements/ForLoop.cpp Language/Statements/IfStatement.cpp Language/Statements/Primary.cpp Language/Statements/ReturnStatement.cpp Language/Statements/Statement.cpp Language/Statements/Untyped.cpp Language/Statements/WhileLoop.cpp Language/TopLevel.cpp Language/TypeDefinitions/Class.cpp Language/TypeDefinitions/Enumeration.cpp Language/TypeDefinitions/Struct.cpp Language/TypeDefinitions/Typedef.cpp Language/TypeDefinitions/TypeDefinition.cpp Language/TypeDefinitions/Union.cpp Language/TypedOwner.cpp Language/TypedStatements/BoolLiteral.cpp Language/TypedStatements/Construction.cpp Language/TypedStatements/Evaluation.cpp Language/TypedStatements/FloatLiteral.cpp Language/TypedStatements/IntegerLiteral.cpp Language/TypedStatements/Invocation.cpp Language/TypedStatements/Literal.cpp Language/TypedStatements/Referenced.cpp Language/TypedStatements/StringLiteral.cpp Language/TypedStatements/ThisPointer.cpp Language/TypedStatements/Typed.cpp Language/TypeNamer.cpp Language/Types/AddressType.cpp Language/Types/Array.cpp Language/Types/Const.cpp Language/Types/ExplicitType.cpp Language/Types/FunctionType.cpp Language/Types/HashType.cpp Language/Types/ListType.cpp Language/Types/Memberify.cpp Language/Types/MemberTemplateType.cpp Language/Types/ModifyingType.cpp Language/Types/Pointer.cpp Language/Types/Reference.cpp Language/Types/SimpleType.cpp Language/Types/StringType.cpp Language/Types/TypeEntity.cpp Language/Types/UndefinedArray.cpp Language/ValueDefiner.cpp Language/ValueDefinitions/BasicOperator.cpp Language/ValueDefinitions/Callable.cpp Language/ValueDefinitions/Constructor.cpp Language/ValueDefinitions/ConversionOperator.cpp Language/ValueDefinitions/DefaultConstructor.cpp Language/ValueDefinitions/Destructor.cpp Language/ValueDefinitions/EnumValue.cpp Language/ValueDefinitions/Function.cpp Language/ValueDefinitions/FundamentalOperator.cpp Language/ValueDefinitions/ImplicitAssignmentOperator.cpp Language/ValueDefinitions/ImplicitCopyConstructor.cpp Language/ValueDefinitions/MemberCallable.cpp Language/ValueDefinitions/Method.cpp Language/ValueDefinitions/MethodOperator.cpp Language/ValueDefinitions/SimpleMethod.cpp Language/ValueDefinitions/SimpleOperator.cpp Language/ValueDefinitions/Variable.cpp Language/ValueDefinitions/VirtualMethod.cpp Language/ValueDefinitions/VirtualOverload.cpp Language/ValueDefinitions/VirtualPure.cpp Language/VirtualBase.cpp Main/DeclarationFile.cpp Main/DeclarationsHandler.cpp Main/FilesExtractorHandler.cpp Main/GccXml.cpp Main/IncludeProject.cpp Main/Project.cpp Support/AuxilliaryRegistrar.cpp Support/Common.cpp Support/CommonGraphics.cpp Support/CompletionDelegate.cpp Support/EditDelegate.cpp Support/EntityKeyEvent.cpp Support/InsertionPoint.cpp Support/Kind.cpp Support/Location.cpp Support/Meta.cpp Support/ModelPtr.cpp Support/ModelPtrFace.cpp Support/Operator.cpp Support/Type.cpp
SOURCES = $$ALL_SOURCES

HEADERS = Precompiled.h tests.h Gui/CodeScene.h Gui/MainWindow.h Language/Artificial.h Language/Base.h Language/Declarations/DeclarationEntity.h Language/Declarations/NamespaceEntity.h Language/Declarations/RootEntity.h Language/Declarations/SubAddressable.h Language/Entity.h Language/Identifiable.h Language/Labels/AccessLabel.h Language/Labels/ConstLabel.h Language/Labels/Label.h Language/Labels/OperatorLabel.h Language/Labels/TextLabel.h Language/Members/Member.h Language/Members/MemberEnumeration.h Language/Members/MemberVariable.h Language/Operations/AssignmentOperation.h Language/Operations/BinaryOperation.h Language/Operations/DereferenceOperation.h Language/Operations/GenericMemberOperation.h Language/Operations/LongMemberOperation.h Language/Operations/MemberOperation.h Language/Operations/NewOperation.h Language/Operations/Operation.h Language/Operations/ReferenceOperation.h Language/Operations/SimpleBinaryOperation.h Language/Operations/SimpleUnaryOperation.h Language/Operations/SubscriptOperation.h Language/Operations/UnaryOperation.h Language/Statements/AssignedVariable.h Language/Statements/BareTyped.h Language/Statements/BreakStatement.h Language/Statements/Compound.h Language/Statements/ConstructedVariable.h Language/Statements/DefaultConstructedVariable.h Language/Statements/DeleteOperation.h Language/Statements/ForLoop.h Language/Statements/IfStatement.h Language/Statements/Primary.h Language/Statements/ReturnStatement.h Language/Statements/Statement.h Language/Statements/Untyped.h Language/Statements/WhileLoop.h Language/TopLevel.h Language/TypeDefinitions/Class.h Language/TypeDefinitions/Enumeration.h Language/TypeDefinitions/Struct.h Language/TypeDefinitions/Typedef.h Language/TypeDefinitions/TypeDefinition.h Language/TypeDefinitions/Union.h Language/TypedOwner.h Language/TypedStatements/BoolLiteral.h Language/TypedStatements/Construction.h Language/TypedStatements/Evaluation.h Language/TypedStatements/FloatLiteral.h Language/TypedStatements/IntegerLiteral.h Language/TypedStatements/Invocation.h Language/TypedStatements/Literal.h Language/TypedStatements/Referenced.h Language/TypedStatements/StringLiteral.h Language/TypedStatements/ThisPointer.h Language/TypedStatements/Typed.h Language/TypeNamer.h Language/Types/AddressType.h Language/Types/Array.h Language/Types/Const.h Language/Types/ExplicitType.h Language/Types/FunctionType.h Language/Types/HashType.h Language/Types/ListType.h Language/Types/Memberify.h Language/Types/MemberTemplateType.h Language/Types/ModifyingType.h Language/Types/Pointer.h Language/Types/Reference.h Language/Types/SimpleType.h Language/Types/StringType.h Language/Types/TypeEntity.h Language/Types/UndefinedArray.h Language/ValueDefiner.h Language/ValueDefinitions/BasicOperator.h Language/ValueDefinitions/Callable.h Language/ValueDefinitions/Constructor.h Language/ValueDefinitions/ConversionOperator.h Language/ValueDefinitions/DefaultConstructor.h Language/ValueDefinitions/Destructor.h Language/ValueDefinitions/EnumValue.h Language/ValueDefinitions/Function.h Language/ValueDefinitions/FundamentalOperator.h Language/ValueDefinitions/ImplicitAssignmentOperator.h Language/ValueDefinitions/ImplicitCopyConstructor.h Language/ValueDefinitions/MemberCallable.h Language/ValueDefinitions/Method.h Language/ValueDefinitions/MethodOperator.h Language/ValueDefinitions/SimpleMethod.h Language/ValueDefinitions/SimpleOperator.h Language/ValueDefinitions/Variable.h Language/ValueDefinitions/VirtualMethod.h Language/ValueDefinitions/VirtualOverload.h Language/ValueDefinitions/VirtualPure.h Language/VirtualBase.h Main/DeclarationFile.h Main/DeclarationsHandler.h Main/FilesExtractorHandler.h Main/GccXml.h Main/IncludeProject.h Main/Project.h Precompiled.h Support/Auxilliary.h Support/AuxilliaryFace.h Support/AuxilliaryRegistrar.h Support/Common.h Support/CommonGraphics.h Support/CompletionDelegate.h Support/DecorationContext.h Support/EditDelegate.h Support/EntityKeyEvent.h Support/InsertionPoint.h Support/Kind.h Support/Location.h Support/Meta.h Support/ModelPtr.h Support/ModelPtrFace.h Support/Operator.h Support/SafePointer.h Support/Type.h
