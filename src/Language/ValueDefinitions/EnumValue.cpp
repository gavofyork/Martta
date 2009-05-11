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
#include "TextLabel.h"
#include "Enumeration.h"
#include "EnumValue.h"

namespace Martta
{

MARTTA_OBJECT_CPP(EnumValue);	
		
bool EnumValue::keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e)
{
	return simpleInsertionPointKeyPressHandler<EnumValue>(_p, _e, "V");
}

Kinds EnumValue::allowedKinds(int _i) const
{
	if (_i == 0)
		return Kind::of<TextLabel>();
	else
		return Kind::of<Typed>();
}

bool EnumValue::isChildInValidState(int _i) const
{
	// Nothing to say about it as it's not typed. If it's supposed to be typed, then the allowed kinds system should pick up the error.
	if (!entityIs<Typed>(_i))
		return true;
	foreach (Type i, Types() << Type(Int))
		if (entityAs<Typed>(_i)->type().isSimilarTo(i, TypeEntity::BasicallyConvertible))
			return true;
	return false;
}

void EnumValue::nameChanged()
{
	if (contextIs<Enumeration>())
		contextAs<Enumeration>()->updateStem();
}

QString EnumValue::defineLayout(ViewKeys&) const
{
	if (entityCount() == 1)
		return "0;";
	else
		return "0;' = ';1";
}

QString EnumValue::code() const
{
	if (entityCount() == 2 && entityIs<Typed>(1))
		return codeName() + " = " + entityAs<Typed>(1)->code();
	else
		return codeName();
}

bool EnumValue::keyPressed(EntityKeyEvent const* _e)
{
	if (_e->text() == "=" && _e->focalIndex() == 0)
	{
		if (entityCount() == 1)
			back().spawnPrepared();
		entity(1)->setCurrent();
	}
	else
		return false;
	return true;
}

void EnumValue::onContextChanged(Entity* _old, Entity* _new)
{
	if (_old->isKind<Enumeration>())
		_old->asKind<Enumeration>()->updateStem();
	if (_new->isKind<Enumeration>())
		_new->asKind<Enumeration>()->updateStem();
}

}
