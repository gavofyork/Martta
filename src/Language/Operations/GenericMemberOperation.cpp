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

#include "Reference.h"
#include "FunctionType.h"
#include "Memberify.h"
#include "Const.h"
#include "Referenced.h"
#include "GenericMemberOperation.h"

namespace Martta
{

MARTTA_OBJECT_CPP(GenericMemberOperation);	

Types GenericMemberOperation::allowedTypes(int _index) const
{
	if (_index == 0)
		return Type();
	if (_index == 1 && !scope().isNull())
	{
		Types ret;
		foreach (Type t, BareTyped::allowedTypes())
			ret << t.topWith(Memberify(scope()));
		return ret;
	}
	return Types();
}

Types GenericMemberOperation::deniedTypes(int _index) const
{
	if (_index == 1)
	{
		Types ret;
		foreach (Type t, BareTyped::deniedTypes())
			ret << t.topWith(Memberify(scope()));
		return ret;
	}
	return Types();
}

bool GenericMemberOperation::isChildInValidState(int _index) const
{
	if (_index == 1)
	{
		if (scope().isNull())
			return false;
		// If we somehow managed to end up at a Memberified type despite not being a Referenced, fair play.
		if (entityIs<Referenced>(1) && !scope()->applicableMembers(context()).contains(entityAs<Referenced>(1)->subject()))
			return false;
	}
	return Super::isChildInValidState(_index);
}

Type GenericMemberOperation::type() const
{
	Type st = scope();
	Type rt = rightType();
	if (!rt->isType<Memberify>())
		return Type();
	Memberify* m = rt->asType<Memberify>();
	if (!m->scope() || !st->isSimilarTo(m->scope(), TypeEntity::Physical))
	{
		return Type();
	}
	m->setScope(st);
	// if the memberified thing isn't const, and the memberify is a const, then make the type const.
	if (st->isType<Const>() && !m->child()->isType<Const>() && !m->child()->isType<FunctionType>())
		m->child()->knit<Const>();
	m->unknit();
	return rt;
}

}
