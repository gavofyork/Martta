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

#include "IdentifierSet.h"
#include "VariablePlacer.h"
#include "SubscriptableRegistrar.h"
#include "ValueDefiner.h"
#include "BuiltinType.h"
#include "FunctionType.h"
#include "Reference.h"
#include "Const.h"
#include "Pointer.h"
#include "Memberify.h"
#include "BuiltinMethod.h"
#include "BuiltinOperator.h"
#include "ListType.h"
#include "StringType.h"

namespace Martta
{

MARTTA_PROPER_CPP(StringType);

List<BuiltinMethod*> StringType::s_members;
List<BuiltinOperator*> StringType::s_nonMembers;

class StringTypeSet: public IdentifierSet
{
public:
	StringTypeSet(): m_ourNamed(L"string") {}
	virtual String						setId() const { return L"Martta::StringType"; }
	virtual List<Named*>				identifiableAt(Position const& _p)
	{
		if (canPlaceVariable(_p))
			return List<Named*>() << &m_ourNamed;
		return List<Named*>();
	}
	virtual void						acceptAt(Position const& _pos, Named*, CodeScene* _cs)
	{
		placeVariable(_pos, new StringType, _cs);
	}
	virtual String						defineEditHtml(Named*, String const& _mid)
	{
		return L"<^><span class=\"TypeConcept\">" + StringType().typeHtml(_mid) + L"</span>";
	}
	SimpleNamed m_ourNamed;
};

static StringTypeSet s_stringTypeSet;

Types StringType::subscriptTypes() const
{
	return Type(Unsigned|Int);
}

Type StringType::subscriptsTo(Type const&) const
{
	if (parent()->isKind<Const>())
		return Type(Wchar);
	else
		return Type(Wchar).topWith(Reference());
}

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

	BuiltinMethod::create<StringType>("append", false, sr, c);
	BuiltinMethod::create<StringType>("append", false, sr, w);
	BuiltinMethod::create<StringType>("append", false, sr, ccs);
	BuiltinMethod::create<StringType>("append", false, sr, scr);

	BuiltinMethod::create<StringType>("at", true, w, u);
	BuiltinMethod::create<StringType>("value", true, w, (u, w));	// 0
	BuiltinMethod::create<StringType>("toCString", true, ccs, n);
	BuiltinMethod::create<StringType>("data", false, ws, n);
	BuiltinMethod::create<StringType>("data", true, wcs, n);
	BuiltinMethod::create<StringType>("constData", true, wcs, n);

	BuiltinMethod::create<StringType>("reserve", false, v, u);
	BuiltinMethod::create<StringType>("clear", false, v, n);
	BuiltinMethod::create<StringType>("chop", false, v, u);
	BuiltinMethod::create<StringType>("truncate", false, v, u);

	BuiltinMethod::create<StringType>("isEmpty", true, b, n);
	BuiltinMethod::create<StringType>("length", true, i, n);
	BuiltinMethod::create<StringType>("size", true, i, n);
	BuiltinMethod::create<StringType>("resize", false, v, u);

	BuiltinMethod::create<StringType>("trimmed", true, s, n);
	BuiltinMethod::create<StringType>("simplified", true, s, n);
	BuiltinMethod::create<StringType>("mid", true, s, (u, u));
	BuiltinMethod::create<StringType>("mid", true, s, u);
	BuiltinMethod::create<StringType>("left", true, s, u);
	BuiltinMethod::create<StringType>("right", true, s, u);

	BuiltinMethod::create<StringType>("contains", true, b, scr);
	BuiltinMethod::create<StringType>("contains", true, b, w);
	BuiltinMethod::create<StringType>("count", true, i, n);
	BuiltinMethod::create<StringType>("count", true, i, scr);
	BuiltinMethod::create<StringType>("count", true, i, w);
	BuiltinMethod::create<StringType>("indexOf", true, i, (w, i));		// 0
	BuiltinMethod::create<StringType>("indexOf", true, i, (wcs, i));		// 0
	BuiltinMethod::create<StringType>("indexOf", true, i, (scr, i));		// 0
	BuiltinMethod::create<StringType>("lastIndexOf", true, i, (w, i));		// -1
	BuiltinMethod::create<StringType>("lastIndexOf", true, i, (scr, i));		// -1
	BuiltinMethod::create<StringType>("indexOfNth", true, i, (w, u));
	BuiltinMethod::create<StringType>("indexOfNth", true, i, (scr, u));
	BuiltinMethod::create<StringType>("lastIndexOfNth", true, i, (w, u));
	BuiltinMethod::create<StringType>("lastIndexOfNth", true, i, (scr, u));

	BuiltinMethod::create<StringType>("startsWith", true, b, w);
	BuiltinMethod::create<StringType>("startsWith", true, b, scr);
	BuiltinMethod::create<StringType>("endsWith", true, b, w);
	BuiltinMethod::create<StringType>("endsWith", true, b, scr);

	BuiltinMethod::create<StringType>("fill", false, sr, (w, i));	// -1

	BuiltinMethod::create<StringType>("split", true, ls, w);
	BuiltinMethod::create<StringType>("split", true, ls, scr);
	BuiltinMethod::create<StringType>("section", true, s, (w, i, i));	// -1
	BuiltinMethod::create<StringType>("section", true, s, (scr, i, i));	// -1

	BuiltinMethod::create<StringType>("toUpper", true, s, n);
	BuiltinMethod::create<StringType>("toLower", true, s, n);

	BuiltinMethod::create<StringType>("replace", false, sr, (u, u, scr));
	BuiltinMethod::create<StringType>("replace", false, sr, (u, u, w));
	BuiltinMethod::create<StringType>("replace", false, sr, (w, scr));
	BuiltinMethod::create<StringType>("replace", false, sr, (w, w));
	BuiltinMethod::create<StringType>("replace", false, sr, (scr, scr));
	BuiltinMethod::create<StringType>("insert", false, sr, (u, w));
	BuiltinMethod::create<StringType>("insert", false, sr, (u, scr));
	BuiltinMethod::create<StringType>("prepend", false, sr, ccs);
	BuiltinMethod::create<StringType>("prepend", false, sr, w);
	BuiltinMethod::create<StringType>("prepend", false, sr, scr);
	BuiltinMethod::create<StringType>("remove", false, sr, (i, i));
	BuiltinMethod::create<StringType>("remove", false, sr, w);
	BuiltinMethod::create<StringType>("remove", false, sr, scr);

	BuiltinMethod::create<StringType>("toInt", true, i, (Type(Bool).topWith(Pointer()), i));	// 0,10
	BuiltinMethod::create<StringType>("toUint", true, u, (Type(Bool).topWith(Pointer()), i));	// 0,10
	BuiltinMethod::create<StringType>("toDouble", true, Type(Double), (Type(Bool).topWith(Pointer())));	// 0
	BuiltinMethod::create<StringType>("toFloat", true, Type(Float), (Type(Bool).topWith(Pointer())));	// 0

	// static methods... TODO.
//	SimpleStaticMethod::create<StringType>("number", s, i);
//	SimpleStaticMethod::create<StringType>("number", s, (u, i));
//	SimpleStaticMethod::create<StringType>("number", s, (Type(Double), c, i));

	BuiltinMethod::create<StringType>("arg", false, s, (scr, i, w));	// 0,L' '
	BuiltinMethod::create<StringType>("arg", false, s, (u, i, i, w));	// 0,10,L' '
	BuiltinMethod::create<StringType>("arg", false, s, (w, i, w));	// 0,L' '
	BuiltinMethod::create<StringType>("arg", false, s, (c, i, w));	// 0,'g',-1,L' '
	BuiltinMethod::create<StringType>("arg", false, s, (Type(Double), i, c, i, w));
	BuiltinMethod::create<StringType>("arg", false, s, (i, i, w));	// 0,L' '

	BuiltinOperator::create<StringType>(Operator::PlusEquals, sr, (sr, scr));
	BuiltinOperator::create<StringType>(Operator::PlusEquals, sr, (sr, ccs));
	BuiltinOperator::create<StringType>(Operator::PlusEquals, sr, (sr, c));
	BuiltinOperator::create<StringType>(Operator::PlusEquals, sr, (sr, w));

	BuiltinOperator::create<StringType>(Operator::EqualsEquals, b, (scr, ccs));
	BuiltinOperator::create<StringType>(Operator::BangEquals, b, (scr, ccs));
	BuiltinOperator::create<StringType>(Operator::LessThan, b, (scr, ccs));
	BuiltinOperator::create<StringType>(Operator::GreaterThan, b, (scr, ccs));
	BuiltinOperator::create<StringType>(Operator::LessThanEquals, b, (scr, ccs));
	BuiltinOperator::create<StringType>(Operator::GreaterThanEquals, b, (scr, ccs));

	BuiltinOperator::create<StringType>(Operator::EqualsEquals, b, (scr, scr));
	BuiltinOperator::create<StringType>(Operator::BangEquals, b, (scr, scr));
	BuiltinOperator::create<StringType>(Operator::LessThan, b, (scr, scr));
	BuiltinOperator::create<StringType>(Operator::GreaterThan, b, (scr, scr));
	BuiltinOperator::create<StringType>(Operator::LessThanEquals, b, (scr, scr));
	BuiltinOperator::create<StringType>(Operator::GreaterThanEquals, b, (scr, scr));

	BuiltinOperator::create<StringType>(Operator::EqualsEquals, b, (ccs, scr));
	BuiltinOperator::create<StringType>(Operator::BangEquals, b, (ccs, scr));
	BuiltinOperator::create<StringType>(Operator::LessThan, b, (ccs, scr));
	BuiltinOperator::create<StringType>(Operator::GreaterThan, b, (ccs, scr));
	BuiltinOperator::create<StringType>(Operator::LessThanEquals, b, (ccs, scr));
	BuiltinOperator::create<StringType>(Operator::GreaterThanEquals, b, (ccs, scr));

	BuiltinOperator::create<StringType>(Operator::Plus, s, (scr, scr));
	BuiltinOperator::create<StringType>(Operator::Plus, s, (scr, ccs));
	BuiltinOperator::create<StringType>(Operator::Plus, s, (ccs, scr));
	BuiltinOperator::create<StringType>(Operator::Plus, s, (scr, c));
	BuiltinOperator::create<StringType>(Operator::Plus, s, (c, scr));

	BuiltinType::registerExtra(L"string", staticKind);
	SubscriptableRegistrar::get()->registerKind<StringType>();
}

void StringType::finaliseClass()
{
	SubscriptableRegistrar::get()->unregisterKind<StringType>();
	BuiltinType::unregisterExtra(L"string");

	while (s_members.size())
		s_members.takeLast()->destruct();
	while (s_nonMembers.size())
		s_nonMembers.takeLast()->destruct();
}

Types StringType::assignableTypes() const
{
	return Type(*this).topWith(Const()).topWith(Reference()), Type(Char).topWith(Const()).topWith(Pointer()), Type(Char), Type(Wchar);
}

List<ValueDefiner*> StringType::applicableMembers(Concept const*, bool _isConst) const
{
	List<ValueDefiner*> ret;
	foreach (BuiltinMethod* i, s_members)
		if (i->apparentType()->asType<Memberify>()->isConst() || !_isConst)
			ret += i;
	return ret;
}

bool StringType::defineSimilarityTo(TypeConcept const* _t, Castability _c) const
{
	return _t->isKind<StringType>() ||
		(isAnyConvertible(_c) && _t->isKind<Pointer>() &&
		_t->asKind<Pointer>()->original()->isType<BuiltinType>() &&
			(_t->asKind<Pointer>()->original()->asType<BuiltinType>()->id() == Char ||
			_t->asKind<Pointer>()->original()->asType<BuiltinType>()->id() == Wchar)) ||
		Super::defineSimilarityTo(_t, _c);
}

bool StringType::defineSimilarityFrom(TypeConcept const* _f, Castability _c) const
{
	return (isAnyConvertible(_c) && _f->isKind<Pointer>() &&
		_f->asKind<Pointer>()->original()->isType<BuiltinType>() &&
			(_f->asKind<Pointer>()->original()->asType<BuiltinType>()->id() == Char ||
			_f->asKind<Pointer>()->original()->asType<BuiltinType>()->id() == Wchar)) ||
		Super::defineSimilarityFrom(_f, _c);
}

String StringType::code(String const& _middle) const
{
	return "::MarttaSupport::String" + _middle;
}

String StringType::defineHtml() const
{
	return L"<^><span class=\"TypeConcept\">" + typeHtml(L"string") + L"</span>";
}

}
