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

only=$3

echo "Preparing Martta language build tree."
echo "Root: $root"

function prepare()
{
local name="$1"	
local files="$2"
local depends="$3"

for d in $depends; do
	if [[ ! -e $root/$d ]]; then
		echo "Dependency $d for plugin $name cannot be found."
		exit;
	fi
done

if [[ "x$only" != "x" ]]; then
if [[ "$only" != "$name" ]]; then
return
fi
fi


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
include($name.pri)
SOURCES += $sources
HEADERS += $headers
EOF

cat > $dest/$name.pri << EOF
DEPS += $depends
OURDIRS = $(for i in $paths; do echo $i; done | sort | uniq | xargs)
TWD = \$\$PWD
$(if [[ "x$sources" == "x" ]]; then echo NO_SOURCES = 1; fi)
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
DESTDIR = \$\$BASE/../plugins
EOF

echo "Support file..."
cat > "$root/dep.pri" << EOF
QMAKE_FEATURES *= \$\$PWD
QMAKE_LIBDIR *= \$\$DESTDIR
DEPENDPATH *= \$\$join(OURDIRS, " \$\$TWD/", "\$\$TWD/")
INCLUDEPATH *= \$\$join(OURDIRS, " \$\$TWD/", "\$\$TWD/")
contains(TARGET, \$\$basename(TWD)): QMAKE_POST_LINK += echo \$\${TARGET} \$\$DEPS > \$\${DESTDIR}/\$(TARGET).dep
!contains(TARGET, \$\$basename(TWD)): !contains(NO_SOURCES, 1): LIBS *= -l\$\$basename(TWD)
for(a, DEPS): !contains(DONE, \$\${a}) {
	DONE += \$\${a}
	NO_SOURCES = 0
	include(\$\${BASE}/\$\${a}/\$\${a}.pri)
}
EOF

prepare IdentifierSet "IdentifierSet Support/IdentifierSetRegistrar" 
prepare Operator "Operator Support/OperatorRegistrar" 

prepare Entity "Support/Auxilliary Support/AuxilliaryFace Support/AuxilliaryRegistrar Support/ChangeMan Support/CodeScene Support/CompletionDelegate Support/CullManager Support/Dier Support/EditDelegate Support/EntitySupport Support/KeyEvent Support/Kind Support/Meta Support/Position Support/SafePointer Support/Stylist Interfaces/ChildValidifier Interfaces/Dependee Interfaces/Depender Interfaces/Familial Entity" 

prepare Named Named Entity
prepare Corporal Corporal Entity
prepare Conditional Conditional Entity

prepare NameEntryPoint NameEntryPoint "Entity Named IdentifierSet"

prepare Label Label Entity
prepare MiscLabels "ConstLabel AccessLabel" Label
prepare IdLabel IdLabel Label
prepare TextLabel "TextLabel Interfaces/Labelled" IdLabel
prepare OperatorLabel OperatorLabel "IdLabel Operator"
prepare Labels "" "TextLabel MiscLabels OperatorLabel"

prepare Identifiable "Identifiable Support/ModelPtr Support/ModelPtrFace Support/ModelPtrRegistrar" "Named IdLabel"

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
prepare Statement "Statement Primary BareTyped Typed Untyped" "NameEntryPoint TypeEntity ValueDefiner"
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

prepare Referenced Referenced "Statement TextLabel IdentifierSet"
prepare LocalReferenced LocalReferenced Referenced
prepare GlobalReferenced GlobalReferenced Referenced
prepare InScopeReferenced InScopeReferenced "Referenced Declaration"

prepare TopLevel TopLevel "Declaration TextLabel"
prepare Root Root "TopLevel Declaration"
prepare TopLevelType TopLevelType "TypeEntity ExplicitType TopLevel"

prepare VariableNamer VariableNamer "QualifierTypes IdLabel TextLabel ValueDefiner"
prepare Argument Argument "VariableNamer TypeEntity TextLabel Declaration"
prepare CQualifiers CQualifiers ""
prepare LambdaNamer LambdaNamer "Argument Compound Statement FunctionType QualifierTypes IdLabel CQualifiers"
prepare ReturnStatement ReturnStatement "LambdaNamer Statement BuiltinType"
prepare StatementVariables "AssignedVariable DefaultConstructedVariable" "VariableNamer Statement TypeEntity"

prepare ArgumentReferenced ArgumentReferenced "Referenced LambdaNamer Declaration"

prepare EnumerationNamer "EnumerationNamer EnumValue" "TextLabel ValueDefiner ExplicitType Declaration BuiltinType Statement"

prepare Variable Variable "CQualifiers VariableNamer TopLevel"
prepare Function Function "CQualifiers LambdaNamer TopLevel"

prepare Namespace Namespace "TopLevel TopLevelType"

prepare Struct Struct TopLevelType
prepare Typedef Typedef "TopLevelType Struct"
prepare Union Union TopLevelType
prepare Enumeration Enumeration "EnumerationNamer TopLevelType"

prepare Member Member "Labels ExplicitType Declaration"
prepare MemberEnumeration MemberEnumeration "Member EnumerationNamer"
prepare MemberValue MemberValue "Member ValueDefiner QualifierTypes Memberify"
prepare MemberVariable MemberVariable "MemberValue VariableNamer"
prepare MemberLambda MemberLambda "ExplicitType AddressType QualifierTypes Compound Argument MemberValue LambdaNamer"

prepare ThisPointer ThisPointer "Statement MemberLambda"

prepare Method Method MemberLambda
prepare MethodOperator MethodOperator "MemberLambda Operator Labels BuiltinType"
prepare ConversionOperator ConversionOperator "MemberLambda Labels"
prepare Constructor Constructor "MemberLambda ExplicitType Labels QualifierTypes"
prepare Construction Construction "Invocation Constructor ExplicitType QualifierTypes"
prepare ConstructedVariable ConstructedVariable "VariableNamer Statement TypeEntity Construction"

prepare Destructor Destructor MemberLambda


prepare Artificials "Interfaces/Artificial ArtificialAssignmentOperator ArtificialCopyConstructor ArtificialDefaultConstructor" "Constructor MethodOperator QualifierTypes ExplicitType"

prepare Class "Class Base VirtualBase Virtual VirtualMethod VirtualOverload VirtualPure" "TopLevelType Memberify Destructor Method MethodOperator ConversionOperator Constructor Destructor MemberEnumeration MemberVariable Artificials"

prepare MemberReferenced MemberReferenced "Class Referenced MemberOperations"
