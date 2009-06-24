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
#include "RootEntity.h"
#include "Memberify.h"
#include "MemberTemplateType.h"
#include "SimpleMethod.h"
#include "SimpleOperator.h"
#include "ListType.h"

namespace Martta
{

MARTTA_OBJECT_CPP(ListType);

QList<SimpleMethod*> ListType::s_members;
QList<SimpleOperator*> ListType::s_nonMembers;

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
	RootEntity* root = RootEntity::get();
	// TODO: Give parameters some names!
	SimpleMethod::create<ListType>("append", false, Type(Void), tcr, root);
	SimpleMethod::create<ListType>("at", false, tcr, Type(Int), root);
	SimpleMethod::create<ListType>("clear", false, Type(Void), Types(), root);
	SimpleMethod::create<ListType>("contains", true, b, tcr, root);
	SimpleMethod::create<ListType>("count", true, Type(Int), tcr, root);
	SimpleMethod::create<ListType>("count", true, Type(Int), Types(), root);
	SimpleMethod::create<ListType>("first", false, tr, Types(), root);
	SimpleMethod::create<ListType>("first", true, tcr, Types(), root);
	SimpleMethod::create<ListType>("indexOf", true, Type(Int), (tcr, Type(Int)), root);		// 0
	SimpleMethod::create<ListType>("insert", false, it, (Type(Int), tcr), root);
	SimpleMethod::create<ListType>("isEmpty", true, b, Types(), root);
	SimpleMethod::create<ListType>("last", false, tr, Types(), root);
	SimpleMethod::create<ListType>("last", true, tcr, Types(), root);
	SimpleMethod::create<ListType>("lastIndexOf", true, Type(Int), (tcr, Type(Int)), root);	// -1
	SimpleMethod::create<ListType>("mid", true, lt, (Type(Int), Type(Int)), root);			// -1
	SimpleMethod::create<ListType>("move", false, Type(Void), (Type(Int), Type(Int)), root);
	SimpleMethod::create<ListType>("prepend", false, Type(Void), tcr, root);
	SimpleMethod::create<ListType>("removeAll", false, Type(Int), tcr, root);
	SimpleMethod::create<ListType>("removeAt", false, Type(Void), Type(Int), root);
	SimpleMethod::create<ListType>("removeFirst", false, Type(Void), Types(), root);
	SimpleMethod::create<ListType>("removeLast", false, Type(Void), Types(), root);
	SimpleMethod::create<ListType>("removeOne", false, b, tcr, root);
	SimpleMethod::create<ListType>("replace", false, Type(Int), tcr, root);
	SimpleMethod::create<ListType>("reserve", false, Type(Void), Type(Int), root);
	SimpleMethod::create<ListType>("reversed", true, lt, Types(), root);
	SimpleMethod::create<ListType>("reverse", false, ltr, Types(), root);
	SimpleMethod::create<ListType>("swap", false, Type(Void), (Type(Int), Type(Int)), root);
	SimpleMethod::create<ListType>("takeAt", false, t, Type(Int), root);
	SimpleMethod::create<ListType>("takeFirst", false, t, Types(), root);
	SimpleMethod::create<ListType>("takeLast", false, t, Types(), root);
	SimpleMethod::create<ListType>("value", false, t, Type(Int), root);
	SimpleMethod::create<ListType>("value", false, t, (Type(Int), tcr), root);

	SimpleOperator::create<ListType>(Operator::PlusEquals, ltr, (ltr, ltcr), root);
	SimpleOperator::create<ListType>(Operator::PlusEquals, ltr, (ltr, tcr), root);
	SimpleOperator::create<ListType>(Operator::LeftShift, ltr, (ltr, ltcr), root);
	SimpleOperator::create<ListType>(Operator::LeftShift, ltr, (ltr, tcr), root);

	SimpleOperator::create<ListType>(Operator::EqualsEquals, b, (ltcr, ltcr), root);
	SimpleOperator::create<ListType>(Operator::BangEquals, b, (ltcr, ltcr), root);
	
	SimpleOperator::create<ListType>(Operator::Plus, lt, (ltcr, ltcr), root);
}

void ListType::finaliseClass()
{
	while (s_members.size())
		delete s_members.takeLast();
	while (s_nonMembers.size())
		delete s_nonMembers.takeLast();
}

Types ListType::assignableTypes() const
{
	return Type(*this);
}

QList<ValueDefiner*> ListType::applicableMembers(Entity*, bool _isConst) const
{
	QList<ValueDefiner*> ret;
	foreach (SimpleMethod* i, s_members)
		if (i->type()->asType<Memberify>()->isConst() || !_isConst)
			ret += i;
	return ret;
}

bool ListType::defineSimilarityFrom(TypeEntity const* _f, Castability _c) const
{
	return _f->isKind<ListType>() && (Type(*original()).isNull() || _f->asKind<ListType>()->original()->isEquivalentTo(original())) ||
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

QString ListType::code(QString const& _middle) const
{
	if (childIs<TypeEntity>(Original))
		return "::MarttaSupport::List< " + childAs<TypeEntity>(Original)->code() + " >" + _middle;
	else
		return "::MarttaSupport::List<>" + _middle;
}

QString ListType::defineLayout(ViewKeys&) const
{
	return ("ycode;%1;^;" + typeLayout() + "'[[]]'").arg(Original);
}

}
