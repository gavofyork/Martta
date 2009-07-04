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

#include "Type.h"
#include "Typed.h"
#include "TypedOwner.h"

namespace Martta
{

MARTTA_INTERFACE_CPP(TypedOwner);	

Types TypedOwner::allowedTypes(int) const
{
	return Types();
}

Types TypedOwner::deniedTypes(int) const
{
	return Types();
}

Type TypedOwner::effectiveType(int _i) const
{
	if (!self()->childIs<Typed>(_i))
		return Type();
	Type p(self()->childAs<Typed>(_i)->type());
	foreach (Type t, allowedTypes(_i))
		if (p.isSimilarTo(t, TypeEntity::Convertible))
//		{
//			mDebug() << p->code() << " is convertible to " << t->code() << " t is ultnull? " << t->isUltimatelyNull();
//			t->debugTree();
			return t->isUltimatelyNull() ? p : t;
//		}
	return Type();
}

Type TypedOwner::nominalType(int _i) const
{
	if (!self()->childIs<Typed>(_i))
		return Type();
	return self()->childAs<Typed>(_i)->type();
}

bool TypedOwner::isChildInValidState(int _i) const
{
//	child(_i)->debugTree();
	// Nothing to say about it as it's not typed. If it's supposed to be typed, then the allowed kinds system should pick up the error.
	if (!self()->childIs<Typed>(_i))
		return true;
	foreach (Type i, deniedTypes(_i))
		if (self()->childAs<Typed>(_i)->type() == i)
			return false;
	foreach (Type i, allowedTypes(_i))
		if (!self()->childIs<Typed>(_i))
		{
			self()->debugTree();
			M_ASSERT(false);
		}
		else
			if (self()->childAs<Typed>(_i)->type().isSimilarTo(i, TypeEntity::Convertible))
				return true;
			else
			{
				mDebug() << self()->childAs<Typed>(_i)->type()->code() << " != " << i->code();
			}
	return false;
}

}
