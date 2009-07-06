CONFIG -= release debug stl

# SET THE CONFIG HERE!
CONFIG += debug warn_on thread qt

profile {
DEFINES += PROFILE
CONFIG += release
}
debug:DEFINES += DEBUG
release:DEFINES += RELEASE

mac:QMAKE_LFLAGS += -Wl,-Sp
unix {
	debug {
		message("Debug build")
		QMAKE_CXXFLAGS_DEBUG += -O0 -g3
	}
	profile {
		message("Profile build")
		QMAKE_CXXFLAGS_RELEASE += -Os -g3
	}
	!profile:release {
		message("Release build")
		QMAKE_CXXFLAGS_RELEASE += -Os -g0
	}
}

#unix:MOC_DIR = /tmp
#unix:RCC_DIR = /tmp
#unix:UI_DIR = /tmp
SUPPORT.version = Versions
SUPPORT.files = ../support/msHash.inl ../support/msList.inl ../support/msDebug.h ../support/msHash.h ../support/msList.h ../support/msRgb.h ../support/msString.h ../support/msStringList.h ../support/msSupport.h ../support/msTextStream.h
SUPPORT.path = Support

ICON = Martta.icns

QMAKE_BUNDLE_DATA += SUPPORT
TEMPLATE = app
TARGET = ../bin/Martta

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
PRECOMPILED_HEADER = Precompiled.h
DISTFILES += ../TODO
RESOURCES += Operators.qrc
#INCLUDEPATH += /usr/local/include/boost-1_35
INCLUDEPATH += Gui Language Language/Declarations Language/Labels Language/Operations Language/Statements Language/TypeDefinitions Language/TypedStatements Language/Types Language/ValueDefinitions Main Support
ALL_SOURCES =  ./Composed.cpp Gui/Composed.cpp Language/Composed.cpp Language/Declarations/Composed.cpp Language/Declarations/Members/Composed.cpp Language/Declarations/Members/Artificial/Composed.cpp Language/Declarations/Members/Virtual/Composed.cpp Language/Declarations/Simple/Composed.cpp Language/Declarations/TopLevel/Composed.cpp Language/Labels/Composed.cpp Language/Statements/Composed.cpp Language/Statements/Typed/Composed.cpp Language/Statements/Typed/Operations/Composed.cpp Language/Types/Composed.cpp Main/Composed.cpp Support/Composed.cpp
SOURCES = $$ALL_SOURCES

HEADERS = Precompiled.h TestHelper.h tests.h Gui/CodeView.h Gui/CommonGraphics.h Gui/MainWindow.h Language/Base.h Language/ChildValidifier.h Language/Conditional.h Language/Corporal.h Language/Declarations/Argument.h Language/Declarations/Declaration.h Language/Declarations/EnumerationNamer.h Language/Declarations/EnumValue.h Language/Declarations/LambdaNamer.h Language/Declarations/Member.h Language/Declarations/Members/Artificial/ArtificialAssignmentOperator.h Language/Declarations/Members/Artificial/ArtificialCopyConstructor.h Language/Declarations/Members/Artificial/ArtificialDefaultConstructor.h Language/Declarations/Members/Artificial.h Language/Declarations/Members/Constructor.h Language/Declarations/Members/ConversionOperator.h Language/Declarations/Members/Destructor.h Language/Declarations/Members/MemberEnumeration.h Language/Declarations/Members/MemberLambda.h Language/Declarations/Members/MemberValue.h Language/Declarations/Members/MemberVariable.h Language/Declarations/Members/Method.h Language/Declarations/Members/MethodOperator.h Language/Declarations/Members/Virtual/VirtualMethod.h Language/Declarations/Members/Virtual/VirtualOverload.h Language/Declarations/Members/Virtual/VirtualPure.h Language/Declarations/Members/Virtual.h Language/Declarations/Root.h Language/Declarations/Simple/SimpleMethod.h Language/Declarations/Simple/SimpleOperator.h Language/Declarations/Simple.h Language/Declarations/TopLevel/Class.h Language/Declarations/TopLevel/Enumeration.h Language/Declarations/TopLevel/Function.h Language/Declarations/TopLevel/Namespace.h Language/Declarations/TopLevel/Struct.h Language/Declarations/TopLevel/TopLevelType.h Language/Declarations/TopLevel/Typedef.h Language/Declarations/TopLevel/TypeDefinition.h Language/Declarations/TopLevel/Union.h Language/Declarations/TopLevel/Variable.h Language/Declarations/TopLevel.h Language/Declarations/TypeNamer.h Language/Declarations/ValueDefiner.h Language/Declarations/VariableNamer.h Language/Dependee.h Language/Depender.h Language/Entity.h Language/Familial.h Language/Identifiable.h Language/Labels/AccessLabel.h Language/Labels/ConstLabel.h Language/Labels/IdLabel.h Language/Labels/Label.h Language/Labels/OperatorLabel.h Language/Labels/TextLabel.h Language/Serialisable.h Language/Statements/AssignedVariable.h Language/Statements/BareTyped.h Language/Statements/BreakStatement.h Language/Statements/Compound.h Language/Statements/ConstructedVariable.h Language/Statements/DefaultConstructedVariable.h Language/Statements/DeleteOperation.h Language/Statements/ForLoop.h Language/Statements/IfStatement.h Language/Statements/Primary.h Language/Statements/ReturnStatement.h Language/Statements/Statement.h Language/Statements/Typed/BoolLiteral.h Language/Statements/Typed/Construction.h Language/Statements/Typed/Evaluation.h Language/Statements/Typed/FloatLiteral.h Language/Statements/Typed/IntegerLiteral.h Language/Statements/Typed/Invocation.h Language/Statements/Typed/Literal.h Language/Statements/Typed/Operations/AssignmentOperation.h Language/Statements/Typed/Operations/BinaryOperation.h Language/Statements/Typed/Operations/DereferenceOperation.h Language/Statements/Typed/Operations/GenericMemberOperation.h Language/Statements/Typed/Operations/LongMemberOperation.h Language/Statements/Typed/Operations/MemberOperation.h Language/Statements/Typed/Operations/NewOperation.h Language/Statements/Typed/Operations/Operation.h Language/Statements/Typed/Operations/ReferenceOperation.h Language/Statements/Typed/Operations/SimpleBinaryOperation.h Language/Statements/Typed/Operations/SimpleUnaryOperation.h Language/Statements/Typed/Operations/SubscriptOperation.h Language/Statements/Typed/Operations/UnaryOperation.h Language/Statements/Typed/Referenced.h Language/Statements/Typed/StringLiteral.h Language/Statements/Typed/ThisPointer.h Language/Statements/Typed/Typed.h Language/Statements/Untyped.h Language/Statements/WhileLoop.h Language/TypedOwner.h Language/Types/AddressType.h Language/Types/Array.h Language/Types/Const.h Language/Types/ExplicitType.h Language/Types/FunctionType.h Language/Types/HashType.h Language/Types/ListType.h Language/Types/Memberify.h Language/Types/MemberTemplateType.h Language/Types/ModifyingType.h Language/Types/Pointer.h Language/Types/Reference.h Language/Types/SimpleType.h Language/Types/StringType.h Language/Types/TypeEntity.h Language/Types/UndefinedArray.h Language/VirtualBase.h Main/DeclarationFile.h Main/DeclarationsHandler.h Main/FilesExtractorHandler.h Main/GccXml.h Main/IncludeProject.h Main/Project.h Main/Timer.h Precompiled.h Support/Auxilliary.h Support/AuxilliaryFace.h Support/AuxilliaryRegistrar.h Support/ChangeMan.h Support/CodeScene.h Support/Common.h Support/CompletionDelegate.h Support/CullManager.h Support/Dier.h Support/EditDelegate.h Support/KeyEvent.h Support/Kind.h Support/Location.h Support/Meta.h Support/ModelPtr.h Support/ModelPtrFace.h Support/ModelPtrRegistrar.h Support/Operator.h Support/Position.h Support/SafePointer.h Support/Stylist.h Support/Type.h TestHelper.h
