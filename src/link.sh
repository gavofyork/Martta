#!/bin/bash

LINK="rsync -t"

DEST=../new_src/Entity
mkdir -p $DEST $DEST/Support $DEST/Interfaces
$LINK Support/Auxilliary.h Support/AuxilliaryFace.h Support/AuxilliaryRegistrar.cpp Support/AuxilliaryRegistrar.h Support/ChangeMan.cpp Support/ChangeMan.h Support/CodeScene.cpp Support/CodeScene.h Support/CompletionDelegate.cpp Support/CompletionDelegate.h Support/CullManager.cpp Support/CullManager.h Support/Dier.cpp Support/Dier.h Support/EditDelegate.cpp Support/EditDelegate.h Support/EntitySupport.h Support/KeyEvent.cpp Support/KeyEvent.h Support/Kind.cpp Support/Kind.h Support/Meta.cpp Support/Meta.h Support/Position.cpp Support/Position.h Support/SafePointer.h Support/Stylist.cpp Support/Stylist.h $DEST/Support/
$LINK Language/ChildValidifier.* Language/Depende* Language/Familial.* $DEST/Interfaces/
$LINK Language/Entity.* $DEST/

DEST=../new_src/TypeEntity
mkdir -p $DEST $DEST/Support $DEST/Interfaces
$LINK Language/Types/TypeEntity.* Language/Types/ModifyingType.* $DEST/
$LINK Language/TypedOwner.* Language/Declarations/TypeNamer.* $DEST/Interfaces/
$LINK Support/Type.* $DEST/Support/

DEST=../new_src/AddressTypes
mkdir -p $DEST
$LINK Language/Types/AddressType.* Language/Types/Pointer.* Language/Types/UndefinedArray.* $DEST/

DEST=../new_src/Identifiable
mkdir -p $DEST $DEST/Support
$LINK Language/Identifiable.* $DEST/
$LINK Support/ModelPtr* $DEST/Support/

DEST=../new_src/Label
mkdir -p $DEST
$LINK Language/Labels/Label.* Language/Labels/IdLabel.* $DEST/

DEST=../new_src/Operator
mkdir -p $DEST $DEST/Support
$LINK Language/Labels/OperatorLabel.* $DEST/
$LINK Support/Operator.* Support/OperatorRegistrar.* $DEST/Support/

DEST=../new_src/ValueDefiner
mkdir -p $DEST
$LINK Language/Declarations/ValueDefiner.* $DEST/

DEST=../new_src/Declaration
mkdir -p $DEST
$LINK Language/Declarations/Declaration.* $DEST/

DEST=../new_src/BasicTypes
mkdir -p $DEST $DEST/Interfaces
$LINK Language/Types/FunctionType.* Language/Types/ExplicitType.* Language/Types/Const.* Language/Types/Reference.* $DEST/
$LINK Language/Declarations/TopLevel/TypeDefinition.* $DEST/Interfaces/

DEST=../new_src/Memberify
mkdir -p $DEST
$LINK Language/Types/Memberify.* $DEST/

DEST=../new_src/BuiltinDeclarations
mkdir -p $DEST
$LINK Language/Declarations/BuiltinDeclaration.* Language/Declarations/Builtin/Builtin* $DEST/

DEST=../new_src/BuiltinTypes
mkdir -p $DEST
$LINK Language/Types/BuiltinType.* $DEST/

DEST=../new_src/Array
mkdir -p $DEST
$LINK Language/Types/Array.* $DEST/

DEST=../new_src/ExtendedTypes
mkdir -p $DEST
$LINK Language/Types/HashType.* $DEST/
$LINK Language/Types/StringType.* Language/Types/ListType.* $DEST/
