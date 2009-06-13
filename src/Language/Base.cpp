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

#include "Class.h"
#include "AccessLabel.h"
#include "ExplicitType.h"
#include "Base.h"

namespace Martta
{

MARTTA_OBJECT_CPP(Base);	

Class* Base::classType() const
{
	if (!isComplete() || !entityAs<ExplicitType>(1)->subject()->isKind<Class>())
		return 0;
	return entityAs<ExplicitType>(1)->subject()->asKind<Class>();
}
	
bool Base::keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e)
{
	return simpleInsertionPointKeyPressHandler<Base>(_p, _e, "B");
}

bool Base::isChildInValidState(int _i) const
{
	if (entityIs<ExplicitType>(_i))
		return Super::isChildInValidState(_i) && entityAs<ExplicitType>(_i)->subject()->isKind<Class>() && entityAs<ExplicitType>(_i)->subject()->self() != context();
	return Super::isChildInValidState(_i);
}

Access Base::access() const
{
	if (!isComplete())
		return NoAccess;
	return entityAs<AccessLabel>(0)->access();
}

QString Base::code() const
{
	if (!isComplete())
		return QString();
	return entityAs<Label>(0)->code() + " " + entityAs<TypeEntity>(1)->code();
}

bool Base::keyPressed(EntityKeyEvent const* _e)
{
	if (entity(0) && entity(0)->keyPressed(_e))
		return true;
	return Super::keyPressed(_e);
}

Kinds Base::allowedKinds(int _i) const
{
	if (_i == 0)
		return Kind::of<AccessLabel>();
	if (_i == 1)
		return Kind::of<ExplicitType>();
	return Kinds();
}

QString Base::defineLayout(ViewKeys&) const
{
	return QString("^;ycode;'inherits';Mi;1;Mi;'as';Mi;0");
}

}
