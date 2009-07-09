#!/bin/bash

if [[ "x$1" == "x" ]]; then
	root="/tmp/src"
	if [[ -e $root ]]; then
		mv $root /tmp/old
		mkdir -p $root
		mv /tmp/old /root
	fi
else
	root="$1"
#	if [[ -e $root ]]; then
#		rm -r $root
#	fi
fi

if [[ "x$2" == "x" ]]; then
	support="../../support"
else
	support="$2"
fi

echo "Preparing Martta language build tree."
echo "Root: $root"

function prepare()
{
local name="$1"	
local files="$2"
local depends="$3"

echo "Preparing plugin $name..."

local dest="$root/$name"
local ffile="*/"
local fpath="/*"
local sources=""
local headers=""
local paths=""

mkdir -p $dest

for f in $files; do
	# split into path/filename
	path=${f/$fpath}
	file=${f/$ffile}
	if [[ $path == $f ]]; then path="."; fi
	paths="$paths $path"
	local header=$(find . -name $file.h)
	local source=$(find . -name $file.cpp)
	mkdir -p $dest/$path
	if [[ -e $header ]]; then
		rsync -t $header $dest/$path/
		headers="$headers $path/$file.h"
	fi
	if [[ -e $source ]]; then
		rsync -t $source $dest/$path/
		sources="$sources $path/$file.cpp"
	fi
done

cat > $dest/$name.pro << EOF
include(../martta.prf)
TARGET = $name
include($name.pri)
SOURCES += $sources
HEADERS += $headers
EOF

cat > $dest/$name.pri << EOF
$(for i in $depends; do echo include\(../$i/$i.pri\); done)
OURDIRS=$(for i in $paths; do echo $i; done | sort | uniq | xargs)
TWD = \$\$PWD
defineTest(hasSources) {
$(if [[ "x$sources" != "x" ]]; then echo return\(true\); else echo return\(false\); fi)
}
include(../dep.pri)
EOF

echo " $name \\" >> $root/Language.pro
}

mkdir -p $root

echo "Main project file..."
cat > "$root/Language.pro" << EOF
TEMPLATE = subdirs
SUBDIRS = \\
EOF

echo "Features file..."
cat > "$root/martta.prf" << EOF
CONFIG -= debug release qt stl exceptions rtti
CONFIG *= debug shared plugin thread
TEMPLATE = lib
VERSION = 0.1.0
BASE = \$\$PWD
OBJECTS_DIR = \$\$BASE/build
DEPENDPATH += $support
INCLUDEPATH += $support
QMAKE_LIBDIR += $support
LIBS += -lsupport
EOF

echo "Support file..."
cat > "$root/dep.pri" << EOF
QMAKE_FEATURES *= \$\$PWD
QMAKE_LIBDIR *= \$\$TWD
DEPENDPATH *= \$\$join(OURDIRS, " \$\$TWD/", "\$\$TWD/")
INCLUDEPATH *= \$\$join(OURDIRS, " \$\$TWD/", "\$\$TWD/")
!contains(TARGET, \$\$basename(TWD)):hasSources(): LIBS *= -l\$\$basename(TWD)
EOF

prepare Entity "Support/Auxilliary Support/AuxilliaryFace Support/AuxilliaryRegistrar Support/ChangeMan Support/CodeScene Support/CompletionDelegate Support/CullManager Support/Dier Support/EditDelegate Support/EntitySupport Support/KeyEvent Support/Kind Support/Meta Support/Position Support/SafePointer Support/Stylist Interfaces/ChildValidifier Interfaces/Dependee Interfaces/Depender Interfaces/Familial Entity" 

prepare Operator "Operator Support/OperatorRegistrar" 

prepare Corporal Corporal Entity
prepare Conditional Conditional Entity

prepare Label Label Entity
prepare MiscLabels "ConstLabel AccessLabel" Label
prepare IdLabel IdLabel Label
prepare TextLabel "TextLabel Interfaces/Labelled" IdLabel
prepare OperatorLabel OperatorLabel "IdLabel Operator"
prepare Labels "" "TextLabel MiscLabels OperatorLabel"

prepare Identifiable "Identifiable Support/ModelPtr Support/ModelPtrFace Support/ModelPtrRegistrar" IdLabel

prepare TypeEntity "TypeEntity ModifyingType Interfaces/TypedOwner Interfaces/TypeNamer Support/Type" Entity

prepare Subscriptable "Subscriptable Support/SubscriptableRegistrar" TypeEntity

prepare PhysicalType PhysicalType TypeEntity 
prepare FunctionType FunctionType PhysicalType

prepare ExplicitType "ExplicitType Interfaces/TypeDefinition" "PhysicalType Identifiable"
prepare QualifierTypes "Const Reference" ExplicitType
prepare AddressType "AddressType Pointer UndefinedArray" "QualifierTypes Subscriptable"
prepare Memberify Memberify "FunctionType AddressType"
prepare MemberTemplateType MemberTemplateType Memberify

prepare ValueDefiner "ValueDefiner" "TypeEntity Identifiable"
prepare Statement "Statement Primary BareTyped Typed Untyped" "TypeEntity ValueDefiner"
prepare Compound Compound Statement
prepare Declaration "Declaration" "Identifiable"

prepare BuiltinDeclarations "BuiltinDeclaration BuiltinMethod BuiltinOperator" "ValueDefiner Declaration Memberify Operator"
prepare BuiltinType "BuiltinType" "PhysicalType AddressType BuiltinDeclarations"
prepare Array Array "AddressType Statement BuiltinType"
prepare ExtendedTypes "HashType ListType StringType" "MemberTemplateType BuiltinType Statement Subscriptable"
prepare Types "" "AddressType QualifierTypes ExplicitType FunctionType Memberify MemberTemplateType Array BuiltinType ExtendedTypes"

prepare ProgramFlow "IfStatement Loop ForLoop WhileLoop BreakStatement" "Compound BuiltinType Corporal Conditional"
prepare Literals "Literal BoolLiteral StringLiteral IntegerLiteral FloatLiteral" "Statement BuiltinType"
prepare Evaluation Evaluation Statement
prepare Invocation Invocation "Evaluation BuiltinType"
prepare Operation Operation "Operator Evaluation ValueDefiner Memberify BuiltinType"
prepare UnaryOperation UnaryOperation Operation
prepare AddressOperations "ReferenceOperation DereferenceOperation" "UnaryOperation AddressType"
prepare SimpleUnaryOperation SimpleUnaryOperation "UnaryOperation BuiltinType ValueDefiner"
prepare BinaryOperation BinaryOperation Operation
prepare AssignmentOperation AssignmentOperation "BinaryOperation BuiltinType"
prepare SimpleBinaryOperation SimpleBinaryOperation "BinaryOperation BuiltinType ValueDefiner"
prepare SubscriptOperation SubscriptOperation "BinaryOperation BuiltinType Subscriptable"
prepare LifeOperations "NewOperation DeleteOperation" "UnaryOperation PhysicalType AddressType Array"

prepare MemberOperations "GenericMemberOperation MemberOperation LongMemberOperation" "BinaryOperation QualifierTypes ExplicitType FunctionType AddressType Memberify"
