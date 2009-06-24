#include "main.cpp"
#include "tests.cpp"
#include "Gui/CodeScene.cpp"
#include "Gui/MainWindow.cpp"
#include "Language/Base.cpp"
#include "Language/ChildValidifier.cpp"
#include "Language/Conditional.cpp"
#include "Language/Corporal.cpp"
#include "Language/Declarations/Argument.cpp"
#include "Language/Declarations/DeclarationEntity.cpp"
#include "Language/Declarations/EnumerationNamer.cpp"
#include "Language/Declarations/EnumValue.cpp"
#include "Language/Declarations/LambdaNamer.cpp"
#include "Language/Declarations/Member.cpp"
#include "Language/Declarations/Members/Artificial/ArtificialAssignmentOperator.cpp"
#include "Language/Declarations/Members/Artificial/ArtificialCopyConstructor.cpp"
#include "Language/Declarations/Members/Artificial/ArtificialDefaultConstructor.cpp"
#include "Language/Declarations/Members/Artificial.cpp"
#include "Language/Declarations/Members/Constructor.cpp"
#include "Language/Declarations/Members/ConversionOperator.cpp"
#include "Language/Declarations/Members/Destructor.cpp"
#include "Language/Declarations/Members/MemberEnumeration.cpp"
#include "Language/Declarations/Members/MemberLambda.cpp"
#include "Language/Declarations/Members/MemberValue.cpp"
#include "Language/Declarations/Members/MemberVariable.cpp"
#include "Language/Declarations/Members/Method.cpp"
#include "Language/Declarations/Members/MethodOperator.cpp"
#include "Language/Declarations/Members/Virtual/VirtualMethod.cpp"
#include "Language/Declarations/Members/Virtual/VirtualOverload.cpp"
#include "Language/Declarations/Members/Virtual/VirtualPure.cpp"
#include "Language/Declarations/Members/Virtual.cpp"
#include "Language/Declarations/RootEntity.cpp"
#include "Language/Declarations/Simple/SimpleMethod.cpp"
#include "Language/Declarations/Simple/SimpleOperator.cpp"
#include "Language/Declarations/Simple.cpp"
#include "Language/Declarations/TopLevel/Class.cpp"
#include "Language/Declarations/TopLevel/Enumeration.cpp"
#include "Language/Declarations/TopLevel/Function.cpp"
#include "Language/Declarations/TopLevel/NamespaceEntity.cpp"
#include "Language/Declarations/TopLevel/Struct.cpp"
#include "Language/Declarations/TopLevel/TopLevelType.cpp"
#include "Language/Declarations/TopLevel/Typedef.cpp"
#include "Language/Declarations/TopLevel/TypeDefinition.cpp"
#include "Language/Declarations/TopLevel/Union.cpp"
#include "Language/Declarations/TopLevel/Variable.cpp"
#include "Language/Declarations/TopLevel.cpp"
#include "Language/Declarations/TypeNamer.cpp"
#include "Language/Declarations/ValueDefiner.cpp"
#include "Language/Declarations/VariableNamer.cpp"
#include "Language/Entity.cpp"
#include "Language/Identifiable.cpp"
#include "Language/Labels/AccessLabel.cpp"
#include "Language/Labels/ConstLabel.cpp"
#include "Language/Labels/IdLabel.cpp"
#include "Language/Labels/Label.cpp"
#include "Language/Labels/OperatorLabel.cpp"
#include "Language/Labels/TextLabel.cpp"
#include "Language/SceneLeaver.cpp"
#include "Language/Statements/AssignedVariable.cpp"
#include "Language/Statements/BareTyped.cpp"
#include "Language/Statements/BreakStatement.cpp"
#include "Language/Statements/Compound.cpp"
#include "Language/Statements/ConstructedVariable.cpp"
#include "Language/Statements/DefaultConstructedVariable.cpp"
#include "Language/Statements/DeleteOperation.cpp"
#include "Language/Statements/ForLoop.cpp"
#include "Language/Statements/IfStatement.cpp"
#include "Language/Statements/Primary.cpp"
#include "Language/Statements/ReturnStatement.cpp"
#include "Language/Statements/Statement.cpp"
#include "Language/Statements/Typed/BoolLiteral.cpp"
#include "Language/Statements/Typed/Construction.cpp"
#include "Language/Statements/Typed/Evaluation.cpp"
#include "Language/Statements/Typed/FloatLiteral.cpp"
#include "Language/Statements/Typed/IntegerLiteral.cpp"
#include "Language/Statements/Typed/Invocation.cpp"
#include "Language/Statements/Typed/Literal.cpp"
#include "Language/Statements/Typed/Operations/AssignmentOperation.cpp"
#include "Language/Statements/Typed/Operations/BinaryOperation.cpp"
#include "Language/Statements/Typed/Operations/DereferenceOperation.cpp"
#include "Language/Statements/Typed/Operations/GenericMemberOperation.cpp"
#include "Language/Statements/Typed/Operations/LongMemberOperation.cpp"
#include "Language/Statements/Typed/Operations/MemberOperation.cpp"
#include "Language/Statements/Typed/Operations/NewOperation.cpp"
#include "Language/Statements/Typed/Operations/Operation.cpp"
#include "Language/Statements/Typed/Operations/ReferenceOperation.cpp"
#include "Language/Statements/Typed/Operations/SimpleBinaryOperation.cpp"
#include "Language/Statements/Typed/Operations/SimpleUnaryOperation.cpp"
#include "Language/Statements/Typed/Operations/SubscriptOperation.cpp"
#include "Language/Statements/Typed/Operations/UnaryOperation.cpp"
#include "Language/Statements/Typed/Referenced.cpp"
#include "Language/Statements/Typed/StringLiteral.cpp"
#include "Language/Statements/Typed/ThisPointer.cpp"
#include "Language/Statements/Typed/Typed.cpp"
#include "Language/Statements/Untyped.cpp"
#include "Language/Statements/WhileLoop.cpp"
#include "Language/TypedOwner.cpp"
#include "Language/Types/AddressType.cpp"
#include "Language/Types/Array.cpp"
#include "Language/Types/Const.cpp"
#include "Language/Types/ExplicitType.cpp"
#include "Language/Types/FunctionType.cpp"
#include "Language/Types/HashType.cpp"
#include "Language/Types/ListType.cpp"
#include "Language/Types/Memberify.cpp"
#include "Language/Types/MemberTemplateType.cpp"
#include "Language/Types/ModifyingType.cpp"
#include "Language/Types/Pointer.cpp"
#include "Language/Types/Reference.cpp"
#include "Language/Types/SimpleType.cpp"
#include "Language/Types/StringType.cpp"
#include "Language/Types/TypeEntity.cpp"
#include "Language/Types/UndefinedArray.cpp"
#include "Language/VirtualBase.cpp"
#include "Main/DeclarationFile.cpp"
#include "Main/DeclarationsHandler.cpp"
#include "Main/FilesExtractorHandler.cpp"
#include "Main/GccXml.cpp"
#include "Main/IncludeProject.cpp"
#include "Main/Project.cpp"
#include "Support/AuxilliaryRegistrar.cpp"
#include "Support/Common.cpp"
#include "Support/CommonGraphics.cpp"
#include "Support/CompletionDelegate.cpp"
#include "Support/EditDelegate.cpp"
#include "Support/EntityKeyEvent.cpp"
#include "Support/InsertionPoint.cpp"
#include "Support/Kind.cpp"
#include "Support/Location.cpp"
#include "Support/Meta.cpp"
#include "Support/ModelPtr.cpp"
#include "Support/ModelPtrFace.cpp"
#include "Support/Operator.cpp"
#include "Support/Type.cpp"

