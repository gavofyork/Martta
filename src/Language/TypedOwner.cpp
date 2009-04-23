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

#include <QtCore>

#include "Common.h"
#include "Const.h"
#include "Typed.h"
#include "TypedOwner.h"

namespace Martta
{

MARTTA_OBJECT_CPP(TypedOwner);	

Types TypedOwner::allowedTypes(int) const
{
	return Type();//Type(Void).topWith(Const());
}

Types TypedOwner::deniedTypes(int) const
{
	return Types();
}

Kinds TypedOwner::deniedKinds(int) const
{
	return context() ? context()->deniedKinds(contextIndex()) : Kinds();
};

Type TypedOwner::effectiveType(int _i) const
{
	if (!entityIs<Typed>(_i))
		return Type();
	Type p(entityAs<Typed>(_i)->type());
	foreach (Type t, allowedTypes(0))
		if (p.isSimilarTo(t, TypeEntity::Convertible))
			return t->isUltimatelyNull() ? p : t;
	return Type();
}

bool TypedOwner::isChildInValidState(int _i) const
{
	// Nothing to say about it as it's not typed. If it's supposed to be typed, then the allowed kinds system should pick up the error.
	if (!entityIs<Typed>(_i))
		return true;
	foreach (Type i, deniedTypes(_i))
		if (entityAs<Typed>(_i)->type() == i)
			return false;
	foreach (Type i, allowedTypes(_i))
		if (!entityIs<Typed>(_i))
		{
			debugTree();
			M_ASSERT(false);
		}
		else
			if (entityAs<Typed>(_i)->type().isSimilarTo(i, TypeEntity::Convertible))
				return true;
	return false;
}

}
