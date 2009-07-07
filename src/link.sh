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
$LINK Language/TypedOwner.* Language/Declarations/TypeNamer.* Language/Declarations/TopLevel/TypeDefinition.* $DEST/Interfaces/
$LINK Support/Type.* $DEST/Support/

DEST=../new_src/Identifiable
mkdir -p $DEST $DEST/Support
$LINK Language/Identifiable.* $DEST/
$LINK Support/ModelPtr* $DEST/Support/

DEST=../new_src/Label
mkdir -p $DEST
$LINK Language/Labels/Label.* Language/Labels/IdLabel.* $DEST/

DEST=../new_src/OperatorLabel
mkdir -p $DEST $DEST/Support
$LINK Language/Labels/OperatorLabel.* $DEST/
$LINK Support/Operator.* $DEST/Support/
