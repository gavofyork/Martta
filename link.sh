#!/bin/bash

# For windows, this is obligatory.
COMPOSED=1

if [[ "x$1" == "x" ]]; then
	src="$PWD/Language"
else
	src="$1"
fi

if [[ "x$2" == "x" ]]; then
	root="$PWD/Language"
else
	root="$2"
fi

if [[ "$src" == "$root" ]]; then
	if [[ -e "/tmp/src" ]]; then
		mv -f /tmp/old /tmp/nold
		mv /tmp/src /tmp/old
		mv -f /tmp/nold /tmp/old
	fi
	mv $src /tmp/src
	src=/tmp/src
else
	if [[ -e $root ]]; then
		mv -f /tmp/oldroot /tmp/older
		mv $root /tmp/oldroot
		mv -f /tmp/older /tmp/oldroot
	fi
fi

if [[ "x$3" == "x" ]]; then
	support="../../support"
else
	support="$3"
fi

echo "Preparing Martta language build tree."
echo "Source: $src"
echo "Dest: $root"

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
local distfiles=""
local data=""
local paths=""

mkdir -p $dest

for f in $files; do
	# split into path/filename
	path=${f/$fpath}
	file=${f/$ffile}
	if [[ $path == $f ]]; then path="."; fi
	paths="$paths $path"
	local header=$(find $src -type f -name $file.h | head -n1)
	local source=$(find $src -type f -name $file.cpp | head -n1)
	local basic=$(find $src -type f -name $file | head -n1)
	mkdir -p $dest/$path
	if [[ -e $header ]]; then
		cp -f $header $dest/$path/
		headers="$headers $path/$file.h"
	fi
	if [[ -e $source ]]; then
		cp -f $source $dest/$path/
		sources="$sources $path/$file.cpp"
	fi
	if [[ -e $basic ]]; then
		cp -f $basic $dest/$path/
		data="$data $path/$file"
	fi
done

local nosources=""
if [[ "x$sources" == "x" ]]; then
	nosources=1
fi

# Previously this was only created when there were sources, but windows can't handle compiling a nothing program.
if [[ $COMPOSED ]]; then
	echo "// Auto-generated composed file" > $dest/.$name-composed.cpp
	for i in $sources; do
		echo "#include \"$i\"" >> $dest/.$name-composed.cpp
	done
	echo >> $dest/.$name-composed.cpp
	headers="$headers"
	distfiles="$sources"
	sources=".$name-composed.cpp"
fi

cat > $dest/$name.pro << EOF
include(../martta.prf)
include($name.pri)
HEADERS += $headers
unix:HEADERS += $distfiles
SOURCES += $sources
DEFINES += M_API_$name=M_INAPI
$(for (( i=4 ; i<$#+1 ; i++ )); do echo ${!i}; done;)
EOF
if [[ "x$data" != "x" ]]; then
mds=""
for i in $data
do
	j=${i/\//\\\\}
	mds="$mds copy $j \$\$replace(DESTDIR, \"/\", \"\\\\\")\\\\Data \&\&"
done
cat >> $dest/$name.pro << EOF
INSTALL_DATA.files = $data
INSTALL_DATA.path = Data
INSTALLS += INSTALL_DATA
!win32:DATA.commands = @echo "Copying data..." && mkdir -p "\$\$DESTDIR/Data" && cp \$\$INSTALL_DATA.files "\$\$DESTDIR/Data"
win32:DATA.commands = @echo Copying data.. && md \$\$replace(DESTDIR, "/", "\\\\")\\\\Data\\\\TBD && $mds rd \$\$replace(DESTDIR, "/", "\\\\")\\\\Data\\\\TBD
QMAKE_EXTRA_TARGETS += DATA
PRE_TARGETDEPS += DATA
EOF
fi

cat > $dest/$name.pri << EOF
DEPS += $depends
OURDIRS = $(for i in $paths; do echo $i; done | sort | uniq | xargs)
TWD = \$\$PWD
$(if [[ $nosources == 1 ]]; then echo NO_SOURCES = 1; fi)
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

if [[ ! -e "$root/martta.prf" ]]; then
echo "Features file..."
cat > "$root/martta.prf" << EOF
CONFIG -= debug release qt stl exceptions rtti
CONFIG *= debug shared plugin thread
profile {
DEFINES *= PROFILE
CONFIG *= release
}
debug:DEFINES *= DEBUG
release:DEFINES *= RELEASE
unix:DEFINES += M_UNIX
macx:DEFINES += M_MAC
win32:DEFINES += M_WIN
!macx:unix:DEFINES += M_LINUX
TEMPLATE = lib
!win32:VERSION = 0.1.0
BASE = \$\$PWD
OBJECTS_DIR = \$\$BASE/build
DEPENDPATH += $support
INCLUDEPATH += $support
QMAKE_LIBDIR += $support \$\$BASE/../plugins
LIBS += -lsupport
DESTDIR = \$\$BASE/../plugins
EOF
fi

if [[ ! -e "$root/dep.pri" ]]; then
echo "Support file..."
cat > "$root/dep.pri" << EOF
QMAKE_FEATURES *= \$\$PWD
QMAKE_LIBDIR *= \$\$DESTDIR
DEPENDPATH *= \$\$join(OURDIRS, " \$\$TWD/", "\$\$TWD/")
INCLUDEPATH *= \$\$join(OURDIRS, " \$\$TWD/", "\$\$TWD/")
contains(TARGET, \$\$basename(TWD)): contains(TEMPLATE, lib) {
	QMAKE_POST_LINK += echo \$\${TARGET} \$\$DEPS > \$\${DESTDIR}/\$(TARGET).dep
	macx: QMAKE_LFLAGS += -install_name @rpath/\$(TARGET)
}
!contains(TARGET, \$\$basename(TWD)): !contains(NO_SOURCES, 1): LIBS *= -l\$\$basename(TWD)
for(a, DEPS): !contains(DONE, \$\${a}) {
	DONE += \$\${a}
	NO_SOURCES = 0
	include(\$\${BASE}/\$\${a}/\$\${a}.pri)
}
EOF
fi

prepare Operator "Operator Support/OperatorRegistrar" 
prepare CQualifiers CQualifiers ""
prepare CTypes CTypes ""

prepare Entity "Support/Auxilliary Support/AuxilliaryFace Support/AuxilliaryRegistrar Support/ChangeMan Support/CodeScene Support/CompletionDelegate Support/CullManager Support/Dier Support/EditDelegate Support/EntitySupport Support/KeyEvent Support/Kind Support/Meta Support/Position Support/SafePointer Interfaces/ChildValidifier Interfaces/Dependee Interfaces/Depender Interfaces/Familial Entity Support/SpecialKeys" 

prepare WebView "Interfaces/WebViewable Interfaces/WebInformer Support/WebStylist Support/WebStylistRegistrar" "Entity"

prepare Project "Project Solution" "Entity"

prepare TypeEntity "TypeEntity ModifyingType Interfaces/TypedOwner Interfaces/TypeNamer Support/Type" "Entity WebView"

prepare Labels "Interfaces/Labelled ConstLabel AccessLabel Label MiscLabels IdLabel TextLabel OperatorLabel" "Entity Operator WebView"

prepare Declaration "Support/IdentifierSet Support/IdentifierSetRegistrar Interfaces/Identifiable Support/ModelPtr Support/ModelPtrFace Support/ModelPtrRegistrar Interfaces/Named NameEntryPoint Declaration" "Labels"

prepare Statement "ValueDefiner Interfaces/Corporal Interfaces/Conditional Statement Primary BareTyped Typed Untyped Compound" "TypeEntity Declaration"

prepare SimpleTypes "Const Reference PhysicalType" "TypeEntity"

prepare Variables "Interfaces/VariableNamer Support/VariablePlacer AssignedVariable DefaultConstructedVariable" "Labels Statement SimpleTypes"

prepare Types "Interfaces/TypeDefinition HashType ListType StringType ExplicitType Array BuiltinType BuiltinDeclaration BuiltinMethod BuiltinOperator MemberTemplateType Memberify AddressType Pointer UndefinedArray FunctionType Subscriptable Support/SubscriptableRegistrar" "TypeEntity Statement CTypes Operator Declaration Variables"

prepare Namers "EnumerationNamer EnumValue Argument LambdaNamer" "Labels Declaration Types Statement CQualifiers"

prepare Statements "ReturnStatement Referenced InScopeReferenced GlobalReferenced ArgumentReferenced LocalReferenced BinaryOperation AssignmentOperation SimpleBinaryOperation SubscriptOperation NewOperation DeleteOperation GenericMemberOperation MemberOperation LongMemberOperation SimpleUnaryOperation ReferenceOperation DereferenceOperation UnaryOperation Operation Invocation Evaluation Literal BoolLiteral StringLiteral IntegerLiteral FloatLiteral IfStatement Loop ForLoop WhileLoop BreakStatement Support/in.svg Support/out.svg Support/GlobalReferenced.svg" "Statement Types Labels Declaration Namers"

prepare CStuff "TopLevel Root TopLevelType Function Variable Struct Union Typedef Enumeration Namespace" "Namers"

prepare Projects "CProject CSolution CDependency MainFunction Support/DeclarationsHandler Support/GccXml" "CStuff Project" "CONFIG += qt" "QT = core xml"

prepare Members "Construction Destructor ConstructedVariable Constructor ConversionOperator MethodOperator Method Member MemberLambda MemberVariable MemberValue MemberEnumeration ThisPointer" "Labels Declaration Types Namers Statements Operator Labels"

prepare Class "Class Interfaces/Artificial ArtificialAssignmentOperator ArtificialCopyConstructor ArtificialDefaultConstructor Base VirtualBase Virtual VirtualMethod VirtualOverload VirtualPure MemberReferenced" "CStuff Types Members Statements"


exit

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
prepare BuiltinType "BuiltinType" "PhysicalType AddressType BuiltinDeclarations CTypes"
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
