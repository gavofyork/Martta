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
#include "Memberify.h"
#include "SimpleMethod.h"
#include "SimpleOperator.h"
#include "ListType.h"
#include "StringType.h"

namespace Martta
{

MARTTA_OBJECT_CPP(StringType);

List<SimpleMethod*> StringType::s_members;
List<SimpleOperator*> StringType::s_nonMembers;

void StringType::initialiseClass()
{
	Type s = Type(StringType());
	Type sr = Type(s).topWith(Reference());
	Type scr = Type(s).topWith(Const()).topWith(Reference());
	Type ccs = Type(Char).topWith(Const()).topWith(Pointer());
	Type w = Type(Wchar);
	Type ws = Type(Wchar).topWith(Pointer());
	Type wcs = Type(Wchar).topWith(Const()).topWith(Pointer());
	Type c = Type(Char);
	Type u = Type(Unsigned|Int);
	Type b = Type(Bool);
	Type i = Type(Int);
	Type v = Type(Void);
	Type ls = Type(StringType()).topWith(ListType());
	Types n;
	// TODO: Give parameters some names!
	// TODO: Handle defaults.
	
	SimpleMethod::create<StringType>("append", false, sr, c);
	SimpleMethod::create<StringType>("append", false, sr, w);
	SimpleMethod::create<StringType>("append", false, sr, ccs);
	SimpleMethod::create<StringType>("append", false, sr, scr);
	
	SimpleMethod::create<StringType>("at", true, w, u);
	SimpleMethod::create<StringType>("value", true, w, (u, w));	// 0
	SimpleMethod::create<StringType>("toCString", true, ccs, n);
	SimpleMethod::create<StringType>("data", false, ws, n);
	SimpleMethod::create<StringType>("data", true, wcs, n);
	SimpleMethod::create<StringType>("constData", true, wcs, n);
	
	SimpleMethod::create<StringType>("reserve", false, v, u);
	SimpleMethod::create<StringType>("clear", false, v, n);
	SimpleMethod::create<StringType>("chop", false, v, u);
	SimpleMethod::create<StringType>("truncate", false, v, u);
	
	SimpleMethod::create<StringType>("isEmpty", true, b, n);
	SimpleMethod::create<StringType>("length", true, i, n);
	SimpleMethod::create<StringType>("size", true, i, n);
	SimpleMethod::create<StringType>("resize", false, v, u);
	
	SimpleMethod::create<StringType>("trimmed", true, s, n);
	SimpleMethod::create<StringType>("simplified", true, s, n);
	SimpleMethod::create<StringType>("mid", true, s, (u, u));
	SimpleMethod::create<StringType>("mid", true, s, u);
	SimpleMethod::create<StringType>("left", true, s, u);
	SimpleMethod::create<StringType>("right", true, s, u);
	
	SimpleMethod::create<StringType>("contains", true, b, scr);
	SimpleMethod::create<StringType>("contains", true, b, w);
	SimpleMethod::create<StringType>("count", true, i, n);
	SimpleMethod::create<StringType>("count", true, i, scr);
	SimpleMethod::create<StringType>("count", true, i, w);
	SimpleMethod::create<StringType>("indexOf", true, i, (w, i));		// 0
	SimpleMethod::create<StringType>("indexOf", true, i, (wcs, i));		// 0
	SimpleMethod::create<StringType>("indexOf", true, i, (scr, i));		// 0
	SimpleMethod::create<StringType>("lastIndexOf", true, i, (w, i));		// -1
	SimpleMethod::create<StringType>("lastIndexOf", true, i, (scr, i));		// -1
	SimpleMethod::create<StringType>("indexOfNth", true, i, (w, u));
	SimpleMethod::create<StringType>("indexOfNth", true, i, (scr, u));
	SimpleMethod::create<StringType>("lastIndexOfNth", true, i, (w, u));
	SimpleMethod::create<StringType>("lastIndexOfNth", true, i, (scr, u));

	SimpleMethod::create<StringType>("startsWith", true, b, w);
	SimpleMethod::create<StringType>("startsWith", true, b, scr);
	SimpleMethod::create<StringType>("endsWith", true, b, w);
	SimpleMethod::create<StringType>("endsWith", true, b, scr);
	
	SimpleMethod::create<StringType>("fill", false, sr, (w, i));	// -1
	
	SimpleMethod::create<StringType>("split", true, ls, w);
	SimpleMethod::create<StringType>("split", true, ls, scr);
	SimpleMethod::create<StringType>("section", true, s, (w, i, i));	// -1
	SimpleMethod::create<StringType>("section", true, s, (scr, i, i));	// -1

	SimpleMethod::create<StringType>("toUpper", true, s, n);
	SimpleMethod::create<StringType>("toLower", true, s, n);

	SimpleMethod::create<StringType>("replace", false, sr, (u, u, scr));
	SimpleMethod::create<StringType>("replace", false, sr, (u, u, w));
	SimpleMethod::create<StringType>("replace", false, sr, (w, scr));
	SimpleMethod::create<StringType>("replace", false, sr, (w, w));
	SimpleMethod::create<StringType>("replace", false, sr, (scr, scr));
	SimpleMethod::create<StringType>("insert", false, sr, (u, w));
	SimpleMethod::create<StringType>("insert", false, sr, (u, scr));
	SimpleMethod::create<StringType>("prepend", false, sr, ccs);
	SimpleMethod::create<StringType>("prepend", false, sr, w);
	SimpleMethod::create<StringType>("prepend", false, sr, scr);
	SimpleMethod::create<StringType>("remove", false, sr, (i, i));
	SimpleMethod::create<StringType>("remove", false, sr, w);
	SimpleMethod::create<StringType>("remove", false, sr, scr);

	SimpleMethod::create<StringType>("toInt", true, i, (Type(Bool).topWith(Pointer()), i));	// 0,10
	SimpleMethod::create<StringType>("toUint", true, u, (Type(Bool).topWith(Pointer()), i));	// 0,10
	SimpleMethod::create<StringType>("toDouble", true, Type(Double), (Type(Bool).topWith(Pointer())));	// 0
	SimpleMethod::create<StringType>("toFloat", true, Type(Float), (Type(Bool).topWith(Pointer())));	// 0
	
	// static methods... TODO.
//	SimpleStaticMethod::create<StringType>("number", s, i);	
//	SimpleStaticMethod::create<StringType>("number", s, (u, i));	
//	SimpleStaticMethod::create<StringType>("number", s, (Type(Double), c, i));	
	
	SimpleMethod::create<StringType>("arg", false, s, (scr, i, w));	// 0,L' '
	SimpleMethod::create<StringType>("arg", false, s, (u, i, i, w));	// 0,10,L' '
	SimpleMethod::create<StringType>("arg", false, s, (w, i, w));	// 0,L' '
	SimpleMethod::create<StringType>("arg", false, s, (c, i, w));	// 0,'g',-1,L' '
	SimpleMethod::create<StringType>("arg", false, s, (Type(Double), i, c, i, w));
	SimpleMethod::create<StringType>("arg", false, s, (i, i, w));	// 0,L' '

	SimpleOperator::create<StringType>(Operator::PlusEquals, sr, (sr, scr));
	SimpleOperator::create<StringType>(Operator::PlusEquals, sr, (sr, ccs));
	SimpleOperator::create<StringType>(Operator::PlusEquals, sr, (sr, c));
	SimpleOperator::create<StringType>(Operator::PlusEquals, sr, (sr, w));

	SimpleOperator::create<StringType>(Operator::EqualsEquals, b, (scr, ccs));
	SimpleOperator::create<StringType>(Operator::BangEquals, b, (scr, ccs));
	SimpleOperator::create<StringType>(Operator::LessThan, b, (scr, ccs));
	SimpleOperator::create<StringType>(Operator::GreaterThan, b, (scr, ccs));
	SimpleOperator::create<StringType>(Operator::LessThanEquals, b, (scr, ccs));
	SimpleOperator::create<StringType>(Operator::GreaterThanEquals, b, (scr, ccs));
	
	SimpleOperator::create<StringType>(Operator::EqualsEquals, b, (scr, scr));
	SimpleOperator::create<StringType>(Operator::BangEquals, b, (scr, scr));
	SimpleOperator::create<StringType>(Operator::LessThan, b, (scr, scr));
	SimpleOperator::create<StringType>(Operator::GreaterThan, b, (scr, scr));
	SimpleOperator::create<StringType>(Operator::LessThanEquals, b, (scr, scr));
	SimpleOperator::create<StringType>(Operator::GreaterThanEquals, b, (scr, scr));
	
	SimpleOperator::create<StringType>(Operator::EqualsEquals, b, (ccs, scr));
	SimpleOperator::create<StringType>(Operator::BangEquals, b, (ccs, scr));
	SimpleOperator::create<StringType>(Operator::LessThan, b, (ccs, scr));
	SimpleOperator::create<StringType>(Operator::GreaterThan, b, (ccs, scr));
	SimpleOperator::create<StringType>(Operator::LessThanEquals, b, (ccs, scr));
	SimpleOperator::create<StringType>(Operator::GreaterThanEquals, b, (ccs, scr));
	
	SimpleOperator::create<StringType>(Operator::Plus, s, (scr, scr));
	SimpleOperator::create<StringType>(Operator::Plus, s, (scr, ccs));
	SimpleOperator::create<StringType>(Operator::Plus, s, (ccs, scr));
	SimpleOperator::create<StringType>(Operator::Plus, s, (scr, c));
	SimpleOperator::create<StringType>(Operator::Plus, s, (c, scr));
}

void StringType::finaliseClass()
{
	while (s_members.size())
		s_members.takeLast()->destruct();
	while (s_nonMembers.size())
		s_nonMembers.takeLast()->destruct();
}

Types StringType::assignableTypes() const
{
	return Type(*this).topWith(Const()).topWith(Reference()), Type(Char).topWith(Const()).topWith(Pointer()), Type(Char), Type(Wchar);
}

List<ValueDefiner*> StringType::applicableMembers(Entity*, bool _isConst) const
{
	List<ValueDefiner*> ret;
	foreach (SimpleMethod* i, s_members)
		if (i->type()->asType<Memberify>()->isConst() || !_isConst)
			ret += i;
	return ret;
}

bool StringType::defineSimilarityTo(TypeEntity const* _t, Castability _c) const
{
	return _t->isKind<StringType>() ||
		isAnyConvertible(_c) && _t->isKind<Pointer>() &&
		_t->asKind<Pointer>()->original()->isType<BuiltinType>() &&
			(_t->asKind<Pointer>()->original()->asType<BuiltinType>()->id() == Char ||
			_t->asKind<Pointer>()->original()->asType<BuiltinType>()->id() == Wchar) ||
		Super::defineSimilarityTo(_t, _c);
}

bool StringType::defineSimilarityFrom(TypeEntity const* _f, Castability _c) const
{
	return isAnyConvertible(_c) && _f->isKind<Pointer>() &&
		_f->asKind<Pointer>()->original()->isType<BuiltinType>() &&
			(_f->asKind<Pointer>()->original()->asType<BuiltinType>()->id() == Char ||
			_f->asKind<Pointer>()->original()->asType<BuiltinType>()->id() == Wchar) ||
		Super::defineSimilarityFrom(_f, _c);
}

String StringType::code(String const& _middle) const
{
	return "::MarttaSupport::String" + _middle;
}

String StringType::defineLayout(ViewKeys const&) const
{
	return typeLayout() + "^;'string'";
}

}
