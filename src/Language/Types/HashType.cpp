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
#include "ValueDefinition.h"
#include "FunctionType.h"
#include "Reference.h"
#include "Const.h"
#include "RootEntity.h"
#include "Memberify.h"
#include "MemberTemplateType.h"
#include "SimpleMethod.h"
#include "SimpleOperator.h"
#include "ListType.h"
#include "HashType.h"

namespace Martta
{

MARTTA_OBJECT_CPP(HashType);

QList<SimpleMethod*> HashType::s_members;
QList<SimpleOperator*> HashType::s_nonMembers;

bool HashType::keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e)
{
	return simpleInsertionPointKeyPressHandler<HashType>(_p, _e, "#");
}

void HashType::initialiseClass()
{
	RootEntity* root = RootEntity::get();
	
	Type k = Type(MemberTemplateType(0));
	Type kr = Type(k).topWith(Reference());
	Type kc = Type(k).topWith(Const());
	Type kcr = Type(k).topWith(Const()).topWith(Reference());
	Type t = Type(MemberTemplateType(1));
	Type tr = Type(t).topWith(Reference());
	Type tc = Type(t).topWith(Const());
	Type tcr = Type(t).topWith(Const()).topWith(Reference());
	Type u = Type(Unsigned);
	Type b = Type(Bool);
	Type i = Type(Int);
	Type v = Type(Void);
	Type lk = Type(ListType()).append(k);
	Type lt = Type(ListType()).append(t);
	Type h = Type(HashType()).append(k).append(t);
	Type hr = Type(h).topWith(Reference());
	Type hcr = Type(h).topWith(Const()).topWith(Reference());
	Type it = v;// should be Iterator.
//	Type cit = v;// should be ConstIterator.
	Types n;
	// TODO: Give parameters some names!
	// TODO: Handle defaults.
	
	SimpleMethod::create<HashType>("reserve", false, v, u, root);
	SimpleMethod::create<HashType>("capacity", true, i, n, root);
	SimpleMethod::create<HashType>("squeeze", false, v, n, root);
	SimpleMethod::create<HashType>("resize", false, v, u, root);

	SimpleMethod::create<HashType>("contains", true, b, kcr, root);
	SimpleMethod::create<HashType>("contains", true, b, kcr, root);
	SimpleMethod::create<HashType>("count", true, i, n, root);
	SimpleMethod::create<HashType>("isEmpty", true, b, n, root);
	SimpleMethod::create<HashType>("size", true, i, n, root);
	SimpleMethod::create<HashType>("count", true, i, kcr, root);

	SimpleMethod::create<HashType>("clear", false, v, n, root);
	SimpleMethod::create<HashType>("insert", false, it, (kcr, tcr), root);
	SimpleMethod::create<HashType>("insertMulti", false, it, (kcr, tcr), root);
	SimpleMethod::create<HashType>("remove", false, i, kcr, root);
	SimpleMethod::create<HashType>("removeOne", false, i, kcr, root);
	SimpleMethod::create<HashType>("take", false, t, kcr, root);

	SimpleMethod::create<HashType>("key", true, kc, tcr, root);
	SimpleMethod::create<HashType>("key", true, kc, (tcr, kcr), root);
	SimpleMethod::create<HashType>("keys", true, lk, tcr, root);
	SimpleMethod::create<HashType>("keys", true, lk, n, root);
	SimpleMethod::create<HashType>("uniqueKeys", true, lk, n, root);

	SimpleMethod::create<HashType>("value", true, tc, kcr, root);
	SimpleMethod::create<HashType>("value", true, tc, (kcr, tcr), root);
	SimpleMethod::create<HashType>("values", true, lt, kcr, root);
	SimpleMethod::create<HashType>("values", true, lt, n, root);
	
	SimpleMethod::create<HashType>("unite", false, hr, hcr, root);

	SimpleOperator::create<HashType>(Operator::EqualsEquals, b, (hcr, hcr), root);
	SimpleOperator::create<HashType>(Operator::BangEquals, b, (hcr, hcr), root);
}

void HashType::finaliseClass()
{
	while (s_members.size())
		delete s_members.takeLast();
	while (s_nonMembers.size())
		delete s_nonMembers.takeLast();
}

Types HashType::assignableTypes() const
{
	return Type(*this);
}

QList<ValueDefinition*> HashType::applicableMembers(Entity*, bool _isConst) const
{
	QList<ValueDefinition*> ret;
	foreach (SimpleMethod* i, s_members)
		if (i->type()->asType<Memberify>()->isConst() || !_isConst)
			ret += i;
	return ret;
}

Kinds HashType::allowedKinds(int _i) const
{
	if (_i < 2)
		return Kind::of<TypeEntity>();
	return Kinds();
}

bool HashType::defineSimilarityTo(TypeEntity const* _t, Castability _c) const
{
	HashType const* t = _t->tryKind<HashType>();
	return t && (t->key()->isNull() || key()->isEquivalentTo(t->key())) && (t->value()->isNull() || key()->isEquivalentTo(t->value())) ||
		Super::defineSimilarityTo(_t, _c);
}

QString HashType::code(QString const& _middle) const
{
	if (entityIs<TypeEntity>(0) && entityIs<TypeEntity>(1))
		return "::MarttaSupport::Hash< " + entityAs<TypeEntity>(1)->code() + ", " + entityAs<TypeEntity>(0)->code() + "> " + _middle;
	else
		return "::MarttaSupport::Hash< >" + _middle;
}

QString HashType::defineLayout(ViewKeys&) const
{
	return ";0;" + typeLayout() + "^;'#';1";
}

}
