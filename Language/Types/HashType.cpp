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
#include "BuiltinType.h"
#include "ValueDefiner.h"
#include "FunctionType.h"
#include "Reference.h"
#include "Const.h"
#include "Memberify.h"
#include "MemberTemplateType.h"
#include "BuiltinMethod.h"
#include "BuiltinOperator.h"
#include "ListType.h"
#include "HashType.h"

namespace Martta
{

MARTTA_OBJECT_CPP(HashType);

List<BuiltinMethod*> HashType::s_members;
List<BuiltinOperator*> HashType::s_nonMembers;

bool HashType::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	return simplePositionKeyPressHandler<HashType>(_p, _e, "#");
}

Types HashType::subscriptTypes() const
{
	return Type(*key()).topWith(Const()).topWith(Reference());
}

Type HashType::subscriptsTo(Type const&) const
{
	if (parent()->isKind<Const>())
		return Type(*value());
	else
		return Type(*value()).topWith(Reference());
}

void HashType::initialiseClass()
{
	Type k = Type(MemberTemplateType(KeyType));
	Type kr = Type(k).topWith(Reference());
	Type kc = Type(k).topWith(Const());
	Type kcr = Type(k).topWith(Const()).topWith(Reference());
	Type t = Type(MemberTemplateType(ValueType));
	Type tr = Type(t).topWith(Reference());
	Type tc = Type(t).topWith(Const());
	Type tcr = Type(t).topWith(Const()).topWith(Reference());
	Type u = Type(Unsigned);
	Type b = Type(Bool);
	Type i = Type(Int);
	Type v = Type(Void);
	Type lk = Type(ListType()).place(k);
	Type lt = Type(ListType()).place(t);
	Type h = Type(HashType()).place(k, HashType::KeyType).place(t);
	Type hr = Type(h).topWith(Reference());
	Type hcr = Type(h).topWith(Const()).topWith(Reference());
	Type it = v;// should be Iterator.
//	Type cit = v;// should be ConstIterator.
	Types n;
	// TODO: Give parameters some names!
	// TODO: Handle defaults.

	BuiltinMethod::create<HashType>("reserve", false, v, u);
	BuiltinMethod::create<HashType>("capacity", true, i, n);
	BuiltinMethod::create<HashType>("squeeze", false, v, n);
	BuiltinMethod::create<HashType>("resize", false, v, u);

	BuiltinMethod::create<HashType>("contains", true, b, kcr);
	BuiltinMethod::create<HashType>("contains", true, b, kcr);
	BuiltinMethod::create<HashType>("count", true, i, n);
	BuiltinMethod::create<HashType>("isEmpty", true, b, n);
	BuiltinMethod::create<HashType>("size", true, i, n);
	BuiltinMethod::create<HashType>("count", true, i, kcr);

	BuiltinMethod::create<HashType>("clear", false, v, n);
	BuiltinMethod::create<HashType>("insert", false, it, (kcr, tcr));
	BuiltinMethod::create<HashType>("insertMulti", false, it, (kcr, tcr));
	BuiltinMethod::create<HashType>("remove", false, i, kcr);
	BuiltinMethod::create<HashType>("removeOne", false, i, kcr);
	BuiltinMethod::create<HashType>("take", false, t, kcr);

	BuiltinMethod::create<HashType>("key", true, kc, tcr);
	BuiltinMethod::create<HashType>("key", true, kc, (tcr, kcr));
	BuiltinMethod::create<HashType>("keys", true, lk, tcr);
	BuiltinMethod::create<HashType>("keys", true, lk, n);
	BuiltinMethod::create<HashType>("uniqueKeys", true, lk, n);

	BuiltinMethod::create<HashType>("value", true, tc, kcr);
	BuiltinMethod::create<HashType>("value", true, tc, (kcr, tcr));
	BuiltinMethod::create<HashType>("values", true, lt, kcr);
	BuiltinMethod::create<HashType>("values", true, lt, n);

	BuiltinMethod::create<HashType>("unite", false, hr, hcr);

	BuiltinOperator::create<HashType>(Operator::EqualsEquals, b, (hcr, hcr));
	BuiltinOperator::create<HashType>(Operator::BangEquals, b, (hcr, hcr));

	SubscriptableRegistrar::get()->registerKind<HashType>();
}

void HashType::finaliseClass()
{
	SubscriptableRegistrar::get()->unregisterKind<HashType>();

	while (s_members.size())
		s_members.takeLast()->destruct();
	while (s_nonMembers.size())
		s_nonMembers.takeLast()->destruct();
}

Types HashType::assignableTypes() const
{
	return Type(*this);
}

List<ValueDefiner*> HashType::applicableMembers(Entity const*, bool _isConst) const
{
	List<ValueDefiner*> ret;
	foreach (BuiltinMethod* i, s_members)
		if (i->type()->asType<Memberify>()->isConst() || !_isConst)
			ret += i;
	return ret;
}

Kinds HashType::allowedKinds(int _i) const
{
	if (_i == KeyType)
		return Kind::of<TypeEntity>();
	return Super::allowedKinds(_i);
}

bool HashType::defineSimilarityTo(TypeEntity const* _t, Castability _c) const
{
	HashType const* t = _t->tryKind<HashType>();
	return (t && (t->key()->isNull() || key()->isEquivalentTo(t->key())) && (t->value()->isNull() || key()->isEquivalentTo(t->value()))) ||
		Super::defineSimilarityTo(_t, _c);
}

String HashType::code(String const& _middle) const
{
	if (childIs<TypeEntity>(ValueType) && childIs<TypeEntity>(KeyType))
		return "::MarttaSupport::Hash< " + childAs<TypeEntity>(KeyType)->code() + ", " + childAs<TypeEntity>(ValueType)->code() + "> " + _middle;
	else
		return "::MarttaSupport::Hash< >" + _middle;
}

String HashType::defineHtml() const
{
	return toHtml(child(ValueType)) + typeHtml(L"<^span class=\"symbol\">#</span>") + toHtml(child(KeyType));
}

}
