DIRS = . Gui Language Language/Declarations Language/Declarations/Builtin Language/Declarations/Members Language/Declarations/Members/Artificial Language/Declarations/Members/Virtual Language/Declarations/TopLevel Language/Labels Language/Statements Language/Statements/Typed Language/Statements/Typed/Operations Language/Statements/Typed/Referenced Language/Types Main Support
MARTTA_COMPOSED = ./Composed.cpp Gui/Composed.cpp Language/Composed.cpp Language/Declarations/Composed.cpp Language/Declarations/Builtin/Composed.cpp Language/Declarations/Members/Composed.cpp Language/Declarations/Members/Artificial/Composed.cpp Language/Declarations/Members/Virtual/Composed.cpp Language/Declarations/TopLevel/Composed.cpp Language/Labels/Composed.cpp Language/Statements/Composed.cpp Language/Statements/Typed/Composed.cpp Language/Statements/Typed/Operations/Composed.cpp Language/Statements/Typed/Referenced/Composed.cpp Language/Types/Composed.cpp Main/Composed.cpp Support/Composed.cpp
MARTTA_SOURCES = changeTests.cpp coreTests.cpp main.cpp supportTests.cpp tests.cpp typeTests.cpp Gui/CodeView.cpp Gui/CommonGraphics.cpp Gui/MainWindow.cpp Language/Base.cpp Language/ChildValidifier.cpp Language/Conditional.cpp Language/Corporal.cpp Language/Declarations/Argument.cpp Language/Declarations/Builtin/BuiltinMethod.cpp Language/Declarations/Builtin/BuiltinOperator.cpp Language/Declarations/BuiltinDeclaration.cpp Language/Declarations/Declaration.cpp Language/Declarations/EnumerationNamer.cpp Language/Declarations/EnumValue.cpp Language/Declarations/LambdaNamer.cpp Language/Declarations/Member.cpp Language/Declarations/Members/Artificial/ArtificialAssignmentOperator.cpp Language/Declarations/Members/Artificial/ArtificialCopyConstructor.cpp Language/Declarations/Members/Artificial/ArtificialDefaultConstructor.cpp Language/Declarations/Members/Artificial.cpp Language/Declarations/Members/Constructor.cpp Language/Declarations/Members/ConversionOperator.cpp Language/Declarations/Members/Destructor.cpp Language/Declarations/Members/MemberEnumeration.cpp Language/Declarations/Members/MemberLambda.cpp Language/Declarations/Members/MemberValue.cpp Language/Declarations/Members/MemberVariable.cpp Language/Declarations/Members/Method.cpp Language/Declarations/Members/MethodOperator.cpp Language/Declarations/Members/Virtual/VirtualMethod.cpp Language/Declarations/Members/Virtual/VirtualOverload.cpp Language/Declarations/Members/Virtual/VirtualPure.cpp Language/Declarations/Members/Virtual.cpp Language/Declarations/Root.cpp Language/Declarations/TopLevel/Class.cpp Language/Declarations/TopLevel/Enumeration.cpp Language/Declarations/TopLevel/Function.cpp Language/Declarations/TopLevel/Namespace.cpp Language/Declarations/TopLevel/Struct.cpp Language/Declarations/TopLevel/TopLevelType.cpp Language/Declarations/TopLevel/Typedef.cpp Language/Declarations/TopLevel/TypeDefinition.cpp Language/Declarations/TopLevel/Union.cpp Language/Declarations/TopLevel/Variable.cpp Language/Declarations/TopLevel.cpp Language/Declarations/TypeNamer.cpp Language/Declarations/ValueDefiner.cpp Language/Declarations/VariableNamer.cpp Language/Dependee.cpp Language/Depender.cpp Language/Concept.cpp Language/Familial.cpp Language/Identifiable.cpp Language/IdentifierSet.cpp Language/Labels/AccessLabel.cpp Language/Labels/ConstLabel.cpp Language/Labels/IdLabel.cpp Language/Labels/Label.cpp Language/Labels/Labelled.cpp Language/Labels/OperatorLabel.cpp Language/Labels/TextLabel.cpp Language/Named.cpp Language/NameEntryPoint.cpp Language/Statements/AssignedVariable.cpp Language/Statements/BareTyped.cpp Language/Statements/BreakStatement.cpp Language/Statements/Compound.cpp Language/Statements/ConstructedVariable.cpp Language/Statements/DefaultConstructedVariable.cpp Language/Statements/DeleteOperation.cpp Language/Statements/ForLoop.cpp Language/Statements/IfStatement.cpp Language/Statements/Loop.cpp Language/Statements/Primary.cpp Language/Statements/ReturnStatement.cpp Language/Statements/Statement.cpp Language/Statements/Typed/BoolLiteral.cpp Language/Statements/Typed/Construction.cpp Language/Statements/Typed/Evaluation.cpp Language/Statements/Typed/FloatLiteral.cpp Language/Statements/Typed/IntegerLiteral.cpp Language/Statements/Typed/Invocation.cpp Language/Statements/Typed/Literal.cpp Language/Statements/Typed/Operations/AssignmentOperation.cpp Language/Statements/Typed/Operations/BinaryOperation.cpp Language/Statements/Typed/Operations/DereferenceOperation.cpp Language/Statements/Typed/Operations/GenericMemberOperation.cpp Language/Statements/Typed/Operations/LongMemberOperation.cpp Language/Statements/Typed/Operations/MemberOperation.cpp Language/Statements/Typed/Operations/NewOperation.cpp Language/Statements/Typed/Operations/Operation.cpp Language/Statements/Typed/Operations/ReferenceOperation.cpp Language/Statements/Typed/Operations/SimpleBinaryOperation.cpp Language/Statements/Typed/Operations/SimpleUnaryOperation.cpp Language/Statements/Typed/Operations/Subscriptable.cpp Language/Statements/Typed/Operations/SubscriptableRegistrar.cpp Language/Statements/Typed/Operations/SubscriptOperation.cpp Language/Statements/Typed/Operations/UnaryOperation.cpp Language/Statements/Typed/Referenced/ArgumentReferenced.cpp Language/Statements/Typed/Referenced/GlobalReferenced.cpp Language/Statements/Typed/Referenced/InScopeReferenced.cpp Language/Statements/Typed/Referenced/LocalReferenced.cpp Language/Statements/Typed/Referenced/MemberReferenced.cpp Language/Statements/Typed/Referenced.cpp Language/Statements/Typed/StringLiteral.cpp Language/Statements/Typed/ThisPointer.cpp Language/Statements/Typed/Typed.cpp Language/Statements/Untyped.cpp Language/Statements/WhileLoop.cpp Language/TypedOwner.cpp Language/Types/AddressType.cpp Language/Types/Array.cpp Language/Types/BuiltinType.cpp Language/Types/Const.cpp Language/Types/ExplicitType.cpp Language/Types/FunctionType.cpp Language/Types/HashType.cpp Language/Types/ListType.cpp Language/Types/Memberify.cpp Language/Types/MemberTemplateType.cpp Language/Types/ModifyingType.cpp Language/Types/PhysicalType.cpp Language/Types/Pointer.cpp Language/Types/Reference.cpp Language/Types/StringType.cpp Language/Types/TypeConcept.cpp Language/Types/UndefinedArray.cpp Language/VirtualBase.cpp Main/DeclarationFile.cpp Main/DeclarationsHandler.cpp Main/FilesExtractorHandler.cpp Main/GccXml.cpp Main/IncludeProject.cpp Main/Project.cpp Main/Timer.cpp Support/AuxilliaryRegistrar.cpp Support/ChangeMan.cpp Support/CodeScene.cpp Support/CompletionDelegate.cpp Support/CullManager.cpp Support/Dier.cpp Support/EditDelegate.cpp Support/IdentifierSetRegistrar.cpp Support/KeyEvent.cpp Support/Kind.cpp Support/Meta.cpp Support/ModelPtr.cpp Support/ModelPtrFace.cpp Support/ModelPtrRegistrar.cpp Support/Operator.cpp Support/OperatorRegistrar.cpp Support/Position.cpp Support/Stylist.cpp Support/Type.cpp
MARTTA_HEADERS = Gui/CodeView.h Gui/CommonGraphics.h Gui/MainWindow.h Language/Base.h Language/ChildValidifier.h Language/Conditional.h Language/Corporal.h Language/Declarations/Argument.h Language/Declarations/Builtin/BuiltinMethod.h Language/Declarations/Builtin/BuiltinOperator.h Language/Declarations/BuiltinDeclaration.h Language/Declarations/Declaration.h Language/Declarations/EnumerationNamer.h Language/Declarations/EnumValue.h Language/Declarations/LambdaNamer.h Language/Declarations/Member.h Language/Declarations/Members/Artificial/ArtificialAssignmentOperator.h Language/Declarations/Members/Artificial/ArtificialCopyConstructor.h Language/Declarations/Members/Artificial/ArtificialDefaultConstructor.h Language/Declarations/Members/Artificial.h Language/Declarations/Members/Constructor.h Language/Declarations/Members/ConversionOperator.h Language/Declarations/Members/Destructor.h Language/Declarations/Members/MemberEnumeration.h Language/Declarations/Members/MemberLambda.h Language/Declarations/Members/MemberValue.h Language/Declarations/Members/MemberVariable.h Language/Declarations/Members/Method.h Language/Declarations/Members/MethodOperator.h Language/Declarations/Members/Virtual/VirtualMethod.h Language/Declarations/Members/Virtual/VirtualOverload.h Language/Declarations/Members/Virtual/VirtualPure.h Language/Declarations/Members/Virtual.h Language/Declarations/Root.h Language/Declarations/TopLevel/Class.h Language/Declarations/TopLevel/Enumeration.h Language/Declarations/TopLevel/Function.h Language/Declarations/TopLevel/Namespace.h Language/Declarations/TopLevel/Struct.h Language/Declarations/TopLevel/TopLevelType.h Language/Declarations/TopLevel/Typedef.h Language/Declarations/TopLevel/TypeDefinition.h Language/Declarations/TopLevel/Union.h Language/Declarations/TopLevel/Variable.h Language/Declarations/TopLevel.h Language/Declarations/TypeNamer.h Language/Declarations/ValueDefiner.h Language/Declarations/VariableNamer.h Language/Dependee.h Language/Depender.h Language/Concept.h Language/Familial.h Language/Identifiable.h Language/IdentifierSet.h Language/Labels/AccessLabel.h Language/Labels/ConstLabel.h Language/Labels/IdLabel.h Language/Labels/Label.h Language/Labels/Labelled.h Language/Labels/OperatorLabel.h Language/Labels/TextLabel.h Language/Named.h Language/NameEntryPoint.h Language/Statements/AssignedVariable.h Language/Statements/BareTyped.h Language/Statements/BreakStatement.h Language/Statements/Compound.h Language/Statements/ConstructedVariable.h Language/Statements/DefaultConstructedVariable.h Language/Statements/DeleteOperation.h Language/Statements/ForLoop.h Language/Statements/IfStatement.h Language/Statements/Loop.h Language/Statements/Primary.h Language/Statements/ReturnStatement.h Language/Statements/Statement.h Language/Statements/Typed/BoolLiteral.h Language/Statements/Typed/Construction.h Language/Statements/Typed/Evaluation.h Language/Statements/Typed/FloatLiteral.h Language/Statements/Typed/IntegerLiteral.h Language/Statements/Typed/Invocation.h Language/Statements/Typed/Literal.h Language/Statements/Typed/Operations/AssignmentOperation.h Language/Statements/Typed/Operations/BinaryOperation.h Language/Statements/Typed/Operations/DereferenceOperation.h Language/Statements/Typed/Operations/GenericMemberOperation.h Language/Statements/Typed/Operations/LongMemberOperation.h Language/Statements/Typed/Operations/MemberOperation.h Language/Statements/Typed/Operations/NewOperation.h Language/Statements/Typed/Operations/Operation.h Language/Statements/Typed/Operations/ReferenceOperation.h Language/Statements/Typed/Operations/SimpleBinaryOperation.h Language/Statements/Typed/Operations/SimpleUnaryOperation.h Language/Statements/Typed/Operations/Subscriptable.h Language/Statements/Typed/Operations/SubscriptableRegistrar.h Language/Statements/Typed/Operations/SubscriptOperation.h Language/Statements/Typed/Operations/UnaryOperation.h Language/Statements/Typed/Referenced/ArgumentReferenced.h Language/Statements/Typed/Referenced/GlobalReferenced.h Language/Statements/Typed/Referenced/InScopeReferenced.h Language/Statements/Typed/Referenced/LocalReferenced.h Language/Statements/Typed/Referenced/MemberReferenced.h Language/Statements/Typed/Referenced.h Language/Statements/Typed/StringLiteral.h Language/Statements/Typed/ThisPointer.h Language/Statements/Typed/Typed.h Language/Statements/Untyped.h Language/Statements/WhileLoop.h Language/TypedOwner.h Language/Types/AddressType.h Language/Types/Array.h Language/Types/BuiltinType.h Language/Types/Const.h Language/Types/ExplicitType.h Language/Types/FunctionType.h Language/Types/HashType.h Language/Types/ListType.h Language/Types/Memberify.h Language/Types/MemberTemplateType.h Language/Types/ModifyingType.h Language/Types/PhysicalType.h Language/Types/Pointer.h Language/Types/Reference.h Language/Types/StringType.h Language/Types/TypeConcept.h Language/Types/UndefinedArray.h Language/VirtualBase.h Main/DeclarationFile.h Main/DeclarationsHandler.h Main/FilesExtractorHandler.h Main/GccXml.h Main/IncludeProject.h Main/Project.h Main/Timer.h Precompiled.h Support/Auxilliary.h Support/AuxilliaryFace.h Support/AuxilliaryRegistrar.h Support/ChangeMan.h Support/CodeScene.h Support/CompletionDelegate.h Support/CQualifiers.h Support/CTypes.h Support/CullManager.h Support/Dier.h Support/EditDelegate.h Support/ConceptSupport.h Support/IdentifierSetRegistrar.h Support/KeyEvent.h Support/Kind.h Support/Meta.h Support/ModelPtr.h Support/ModelPtrFace.h Support/ModelPtrRegistrar.h Support/Operator.h Support/OperatorRegistrar.h Support/Position.h Support/SafePointer.h Support/Stylist.h Support/Type.h TestHelper.h
MARTTA_INLINES =
SUPPORT_SOURCES = ../support/msDebug.cpp ../support/msFlags.cpp ../support/msRgb.cpp ../support/msString.cpp ../support/msStringList.cpp ../support/msSupport.cpp ../support/msTextStream.cpp
SUPPORT_HEADERS = ../support/msDebug.h ../support/msFlags.h ../support/msHash.h ../support/msList.h ../support/msRgb.h ../support/msString.h ../support/msStringList.h ../support/msSupport.h ../support/msTextStream.h
SUPPORT_INLINES = ../support/msHash.inl ../support/msList.inl
