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

#include "SubscriptableRegistrar.h"
#include "Typed.h"
#include "ValueDefiner.h"
#include "BuiltinType.h"
#include "FunctionType.h"
#include "Reference.h"
#include "Const.h"
#include "Memberify.h"
#include "MemberTemplateType.h"
#include "BuiltinMethod.h"
#include "BuiltinOperator.h"
#include "ListType.h"

namespace Martta
{

MARTTA_OBJECT_CPP(ListType);

List<BuiltinMethod*> ListType::s_members;
List<BuiltinOperator*> ListType::s_nonMembers;

Types ListType::subscriptTypes() const
{
	return Type(Unsigned|Int);
}

Type ListType::subscriptsTo(Type const&) const
{
	if (parent()->isKind<Const>())
		return Type(*original());
	else
		return Type(*original()).topWith(Reference());
}

void ListType::initialiseClass()
{
	Type t = MemberTemplateType(Original);
	Type tcr = Type(MemberTemplateType(Original)).topWith(Const()).topWith(Reference());
	Type tr = Type(MemberTemplateType(Original)).topWith(Reference());
	Type lt = Type(ListType()).place(MemberTemplateType(Original));
	Type ltr = Type(ListType()).place(MemberTemplateType(Original)).topWith(Reference());
	Type ltcr = Type(ListType()).place(MemberTemplateType(Original)).topWith(Const()).topWith(Reference());
	Type it = Type(Void);	// should be Iterator.
	Type b = Type(Bool);
	// TODO: Give parameters some names!
	BuiltinMethod::create<ListType>("append", false, Type(Void), tcr);
	BuiltinMethod::create<ListType>("at", false, tcr, Type(Int));
	BuiltinMethod::create<ListType>("clear", false, Type(Void), Types());
	BuiltinMethod::create<ListType>("contains", true, b, tcr);
	BuiltinMethod::create<ListType>("count", true, Type(Int), tcr);
	BuiltinMethod::create<ListType>("count", true, Type(Int), Types());
	BuiltinMethod::create<ListType>("first", false, tr, Types());
	BuiltinMethod::create<ListType>("first", true, tcr, Types());
	BuiltinMethod::create<ListType>("indexOf", true, Type(Int), (tcr, Type(Int)));		// 0
	BuiltinMethod::create<ListType>("insert", false, it, (Type(Int), tcr));
	BuiltinMethod::create<ListType>("isEmpty", true, b, Types());
	BuiltinMethod::create<ListType>("last", false, tr, Types());
	BuiltinMethod::create<ListType>("last", true, tcr, Types());
	BuiltinMethod::create<ListType>("lastIndexOf", true, Type(Int), (tcr, Type(Int)));	// -1
	BuiltinMethod::create<ListType>("mid", true, lt, (Type(Int), Type(Int)));			// -1
	BuiltinMethod::create<ListType>("move", false, Type(Void), (Type(Int), Type(Int)));
	BuiltinMethod::create<ListType>("prepend", false, Type(Void), tcr);
	BuiltinMethod::create<ListType>("removeAll", false, Type(Int), tcr);
	BuiltinMethod::create<ListType>("removeAt", false, Type(Void), Type(Int));
	BuiltinMethod::create<ListType>("removeFirst", false, Type(Void), Types());
	BuiltinMethod::create<ListType>("removeLast", false, Type(Void), Types());
	BuiltinMethod::create<ListType>("removeOne", false, b, tcr);
	BuiltinMethod::create<ListType>("replace", false, Type(Int), tcr);
	BuiltinMethod::create<ListType>("reserve", false, Type(Void), Type(Int));
	BuiltinMethod::create<ListType>("reversed", true, lt, Types());
	BuiltinMethod::create<ListType>("reverse", false, ltr, Types());
	BuiltinMethod::create<ListType>("swap", false, Type(Void), (Type(Int), Type(Int)));
	BuiltinMethod::create<ListType>("takeAt", false, t, Type(Int));
	BuiltinMethod::create<ListType>("takeFirst", false, t, Types());
	BuiltinMethod::create<ListType>("takeLast", false, t, Types());
	BuiltinMethod::create<ListType>("value", false, t, Type(Int));
	BuiltinMethod::create<ListType>("value", false, t, (Type(Int), tcr));

	BuiltinOperator::create<ListType>(Operator::PlusEquals, ltr, (ltr, ltcr));
	BuiltinOperator::create<ListType>(Operator::PlusEquals, ltr, (ltr, tcr));
	BuiltinOperator::create<ListType>(Operator::LeftShift, ltr, (ltr, ltcr));
	BuiltinOperator::create<ListType>(Operator::LeftShift, ltr, (ltr, tcr));

	BuiltinOperator::create<ListType>(Operator::EqualsEquals, b, (ltcr, ltcr));
	BuiltinOperator::create<ListType>(Operator::BangEquals, b, (ltcr, ltcr));

	BuiltinOperator::create<ListType>(Operator::Plus, lt, (ltcr, ltcr));

	SubscriptableRegistrar::get()->registerKind<ListType>();
}

void ListType::finaliseClass()
{
	SubscriptableRegistrar::get()->unregisterKind<ListType>();

	while (s_members.size())
		s_members.takeLast()->destruct();
	while (s_nonMembers.size())
		s_nonMembers.takeLast()->destruct();
}

Types ListType::assignableTypes() const
{
	return Type(*this);
}

List<ValueDefiner*> ListType::applicableMembers(Entity const*, bool _isConst) const
{
	List<ValueDefiner*> ret;
	foreach (BuiltinMethod* i, s_members)
		if (i->type()->asType<Memberify>()->isConst() || !_isConst)
			ret += i;
	return ret;
}

bool ListType::defineSimilarityFrom(TypeEntity const* _f, Castability _c) const
{
	return (_f->isKind<ListType>() && (Type(*original()).isNull() || _f->asKind<ListType>()->original()->isEquivalentTo(original()))) ||
		Super::defineSimilarityFrom(_f, _c);
}

Kinds ListType::allowedKinds(int _i) const
{
	if (_i == Length)
		return Kind::of<Typed>();
	return Super::allowedKinds(_i);
}

Types ListType::allowedTypes(int _i) const
{
	if (_i == Length)
		return Type(Int);
	return Super::allowedTypes(_i);
}

String ListType::code(String const& _middle) const
{
	if (childIs<TypeEntity>(Original))
		return "::MarttaSupport::List< " + childAs<TypeEntity>(Original)->code() + " >" + _middle;
	else
		return "::MarttaSupport::List<>" + _middle;
}

String ListType::defineLayout(ViewKeys const&) const
{
	return ("ycode;%1;^;" + typeLayout() + "'[[]]'").arg(Original);
}

}
