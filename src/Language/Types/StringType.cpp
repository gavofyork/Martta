/* ***** BEGIN LICENSE BLOCK *****
 * Version: Martta License version 1.0
 *
 * The contents of this file are subject to the Martta License version 1.0
 * (the "License"); you may not use this file except in compliance with the 
 * License. You should have received a copy of the Martta License 
 * "COPYING.Martta" along with Martta; if not you may obtain a copy of the
 * License at http://quidprocode.co.uk/Martta/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations under 
 * the License.
 *
 * The Initial Developer of the code in this file is Gavin Wood.
 * Portions created by the Initial Developer are Copyright (C) 2008
 * quid pro code, Ltd. All Rights Reserved.
 *
 * ***** END LICENSE BLOCK ***** */

#include "Typed.h"
#include "ValueDefiner.h"
#include "FunctionType.h"
#include "Reference.h"
#include "Const.h"
#include "Pointer.h"
#include "RootEntity.h"
#include "Memberify.h"
#include "SimpleMethod.h"
#include "SimpleOperator.h"
#include "ListType.h"
#include "StringType.h"

namespace Martta
{

MARTTA_OBJECT_CPP(StringType);

QList<SimpleMethod*> StringType::s_members;
QList<ValueDefiner*> StringType::s_nonMembers;

void StringType::initialiseClass()
{
	RootEntity* root = RootEntity::get();
	
	Type s = Type(StringType());
	Type sr = Type(s).topWith(Reference());
	Type scr = Type(s).topWith(Const()).topWith(Reference());
	Type ccs = Type(Char).topWith(Const()).topWith(Pointer());
	Type w = Type(Wchar);
	Type ws = Type(Wchar).topWith(Pointer());
	Type wcs = Type(Wchar).topWith(Const()).topWith(Pointer());
	Type c = Type(Char);
	Type u = Type(Unsigned);
	Type b = Type(Bool);
	Type i = Type(Int);
	Type v = Type(Void);
	Type ls = Type(StringType()).topWith(ListType());
	Types n;
	// TODO: Give parameters some names!
	// TODO: Handle defaults.
	
	SimpleMethod::create<StringType>("append", false, sr, c, root);
	SimpleMethod::create<StringType>("append", false, sr, w, root);
	SimpleMethod::create<StringType>("append", false, sr, ccs, root);
	SimpleMethod::create<StringType>("append", false, sr, scr, root);
	
	SimpleMethod::create<StringType>("at", true, w, u, root);
	SimpleMethod::create<StringType>("value", true, w, (u, w), root);	// 0
	SimpleMethod::create<StringType>("toCString", true, ccs, n, root);
	SimpleMethod::create<StringType>("data", false, ws, n, root);
	SimpleMethod::create<StringType>("data", true, wcs, n, root);
	SimpleMethod::create<StringType>("constData", true, wcs, n, root);
	
	SimpleMethod::create<StringType>("reserve", false, v, u, root);
	SimpleMethod::create<StringType>("clear", false, v, n, root);
	SimpleMethod::create<StringType>("chop", false, v, u, root);
	SimpleMethod::create<StringType>("truncate", false, v, u, root);
	
	SimpleMethod::create<StringType>("isEmpty", true, b, n, root);
	SimpleMethod::create<StringType>("length", true, i, n, root);
	SimpleMethod::create<StringType>("size", true, i, n, root);
	SimpleMethod::create<StringType>("resize", false, v, u, root);
	
	SimpleMethod::create<StringType>("trimmed", true, s, n, root);
	SimpleMethod::create<StringType>("simplified", true, s, n, root);
	SimpleMethod::create<StringType>("mid", true, s, (u, u), root);
	SimpleMethod::create<StringType>("mid", true, s, u, root);
	SimpleMethod::create<StringType>("left", true, s, u, root);
	SimpleMethod::create<StringType>("right", true, s, u, root);
	
	SimpleMethod::create<StringType>("contains", true, b, scr, root);
	SimpleMethod::create<StringType>("contains", true, b, w, root);
	SimpleMethod::create<StringType>("count", true, i, n, root);
	SimpleMethod::create<StringType>("count", true, i, scr, root);
	SimpleMethod::create<StringType>("count", true, i, w, root);
	SimpleMethod::create<StringType>("indexOf", true, i, (w, i), root);		// 0
	SimpleMethod::create<StringType>("indexOf", true, i, (wcs, i), root);		// 0
	SimpleMethod::create<StringType>("indexOf", true, i, (scr, i), root);		// 0
	SimpleMethod::create<StringType>("lastIndexOf", true, i, (w, i), root);		// -1
	SimpleMethod::create<StringType>("lastIndexOf", true, i, (scr, i), root);		// -1
	SimpleMethod::create<StringType>("indexOfNth", true, i, (w, u), root);
	SimpleMethod::create<StringType>("indexOfNth", true, i, (scr, u), root);
	SimpleMethod::create<StringType>("lastIndexOfNth", true, i, (w, u), root);
	SimpleMethod::create<StringType>("lastIndexOfNth", true, i, (scr, u), root);

	SimpleMethod::create<StringType>("startsWith", true, b, w, root);
	SimpleMethod::create<StringType>("startsWith", true, b, scr, root);
	SimpleMethod::create<StringType>("endsWith", true, b, w, root);
	SimpleMethod::create<StringType>("endsWith", true, b, scr, root);
	
	SimpleMethod::create<StringType>("fill", false, sr, (w, i), root);	// -1
	
	SimpleMethod::create<StringType>("split", true, ls, w, root);
	SimpleMethod::create<StringType>("split", true, ls, scr, root);
	SimpleMethod::create<StringType>("section", true, s, (w, i, i), root);	// -1
	SimpleMethod::create<StringType>("section", true, s, (scr, i, i), root);	// -1

	SimpleMethod::create<StringType>("toUpper", true, s, n, root);
	SimpleMethod::create<StringType>("toLower", true, s, n, root);

	SimpleMethod::create<StringType>("replace", false, sr, (u, u, scr), root);
	SimpleMethod::create<StringType>("replace", false, sr, (u, u, w), root);
	SimpleMethod::create<StringType>("replace", false, sr, (w, scr), root);
	SimpleMethod::create<StringType>("replace", false, sr, (w, w), root);
	SimpleMethod::create<StringType>("replace", false, sr, (scr, scr), root);
	SimpleMethod::create<StringType>("insert", false, sr, (u, w), root);
	SimpleMethod::create<StringType>("insert", false, sr, (u, scr), root);
	SimpleMethod::create<StringType>("prepend", false, sr, ccs, root);
	SimpleMethod::create<StringType>("prepend", false, sr, w, root);
	SimpleMethod::create<StringType>("prepend", false, sr, scr, root);
	SimpleMethod::create<StringType>("remove", false, sr, (i, i), root);
	SimpleMethod::create<StringType>("remove", false, sr, w, root);
	SimpleMethod::create<StringType>("remove", false, sr, scr, root);

	SimpleMethod::create<StringType>("toInt", true, i, (Type(Bool).topWith(Pointer()), i), root);	// 0,10
	SimpleMethod::create<StringType>("toUint", true, u, (Type(Bool).topWith(Pointer()), i), root);	// 0,10
	SimpleMethod::create<StringType>("toDouble", true, Type(Double), (Type(Bool).topWith(Pointer())), root);	// 0
	SimpleMethod::create<StringType>("toFloat", true, Type(Float), (Type(Bool).topWith(Pointer())), root);	// 0
	
	// static methods... TODO.
//	SimpleStaticMethod::create<StringType>("number", s, i, root);	
//	SimpleStaticMethod::create<StringType>("number", s, (u, i), root);	
//	SimpleStaticMethod::create<StringType>("number", s, (Type(Double), c, i), root);	
	
	SimpleMethod::create<StringType>("arg", false, s, (scr, i, w), root);	// 0,L' '
	SimpleMethod::create<StringType>("arg", false, s, (u, i, i, w), root);	// 0,10,L' '
	SimpleMethod::create<StringType>("arg", false, s, (w, i, w), root);	// 0,L' '
	SimpleMethod::create<StringType>("arg", false, s, (c, i, w), root);	// 0,'g',-1,L' '
	SimpleMethod::create<StringType>("arg", false, s, (Type(Double), i, c, i, w), root);
	SimpleMethod::create<StringType>("arg", false, s, (i, i, w), root);	// 0,L' '

	SimpleOperator::create<StringType>(Operator::PlusEquals, sr, (sr, scr), root);
	SimpleOperator::create<StringType>(Operator::PlusEquals, sr, (sr, ccs), root);
	SimpleOperator::create<StringType>(Operator::PlusEquals, sr, (sr, c), root);
	SimpleOperator::create<StringType>(Operator::PlusEquals, sr, (sr, w), root);

	SimpleOperator::create<StringType>(Operator::EqualsEquals, b, (scr, ccs), root);
	SimpleOperator::create<StringType>(Operator::BangEquals, b, (scr, ccs), root);
	SimpleOperator::create<StringType>(Operator::LessThan, b, (scr, ccs), root);
	SimpleOperator::create<StringType>(Operator::GreaterThan, b, (scr, ccs), root);
	SimpleOperator::create<StringType>(Operator::LessThanEquals, b, (scr, ccs), root);
	SimpleOperator::create<StringType>(Operator::GreaterThanEquals, b, (scr, ccs), root);
	
	SimpleOperator::create<StringType>(Operator::EqualsEquals, b, (scr, scr), root);
	SimpleOperator::create<StringType>(Operator::BangEquals, b, (scr, scr), root);
	SimpleOperator::create<StringType>(Operator::LessThan, b, (scr, scr), root);
	SimpleOperator::create<StringType>(Operator::GreaterThan, b, (scr, scr), root);
	SimpleOperator::create<StringType>(Operator::LessThanEquals, b, (scr, scr), root);
	SimpleOperator::create<StringType>(Operator::GreaterThanEquals, b, (scr, scr), root);
	
	SimpleOperator::create<StringType>(Operator::EqualsEquals, b, (ccs, scr), root);
	SimpleOperator::create<StringType>(Operator::BangEquals, b, (ccs, scr), root);
	SimpleOperator::create<StringType>(Operator::LessThan, b, (ccs, scr), root);
	SimpleOperator::create<StringType>(Operator::GreaterThan, b, (ccs, scr), root);
	SimpleOperator::create<StringType>(Operator::LessThanEquals, b, (ccs, scr), root);
	SimpleOperator::create<StringType>(Operator::GreaterThanEquals, b, (ccs, scr), root);
	
	SimpleOperator::create<StringType>(Operator::Plus, s, (scr, scr), root);
	SimpleOperator::create<StringType>(Operator::Plus, s, (scr, ccs), root);
	SimpleOperator::create<StringType>(Operator::Plus, s, (ccs, scr), root);
	SimpleOperator::create<StringType>(Operator::Plus, s, (scr, c), root);
	SimpleOperator::create<StringType>(Operator::Plus, s, (c, scr), root);
}

void StringType::finaliseClass()
{
	while (s_members.size())
		delete s_members.takeLast();
	while (s_nonMembers.size())
		delete s_nonMembers.takeLast();
}

Types StringType::assignableTypes() const
{
	return Type(*this).topWith(Const()).topWith(Reference()), Type(Char).topWith(Const()).topWith(Pointer()), Type(Char), Type(Wchar);
}

QList<ValueDefiner*> StringType::applicableMembers(Entity*, bool _isConst) const
{
	QList<ValueDefiner*> ret;
	foreach (SimpleMethod* i, s_members)
		if (i->type()->asType<Memberify>()->isConst() || !_isConst)
			ret += i;
	return ret;
}

bool StringType::defineSimilarityTo(TypeEntity const* _t, Castability _c) const
{
	return _t->isKind<StringType>() ||
		isAnyConvertible(_c) && _t->isKind<Pointer>() &&
		_t->asKind<Pointer>()->child()->isType<SimpleType>() &&
			(_t->asKind<Pointer>()->child()->asType<SimpleType>()->id() == Char ||
			_t->asKind<Pointer>()->child()->asType<SimpleType>()->id() == Wchar) ||
		Super::defineSimilarityTo(_t, _c);
}

bool StringType::defineSimilarityFrom(TypeEntity const* _f, Castability _c) const
{
	return isAnyConvertible(_c) && _f->isKind<Pointer>() &&
		_f->asKind<Pointer>()->child()->isType<SimpleType>() &&
			(_f->asKind<Pointer>()->child()->asType<SimpleType>()->id() == Char ||
			_f->asKind<Pointer>()->child()->asType<SimpleType>()->id() == Wchar) ||
		Super::defineSimilarityFrom(_f, _c);
}

QString StringType::code(QString const& _middle) const
{
	return "::MarttaSupport::String" + _middle;
}

QString StringType::defineLayout(ViewKeys&) const
{
	return typeLayout() + "^;'string'";
}

}
