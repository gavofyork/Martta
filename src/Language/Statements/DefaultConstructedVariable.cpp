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

#include "TextLabel.h"
#include "TypeEntity.h"
#include "AssignedVariable.h"
#include "DefaultConstructedVariable.h"

namespace Martta
{

MARTTA_OBJECT_CPP(DefaultConstructedVariable);	

bool DefaultConstructedVariable::keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e)
{
	return simplePlaceholderKeyPressHandler<DefaultConstructedVariable>(_p, _e, "V");
}

Kinds DefaultConstructedVariable::allowedKinds(int _index) const
{
	if (_index == OurType)
		return Kind::of<TypeEntity>();
	else if (_index == Identity)
		return Kind::of<TextLabel>();
	return Super::allowedKinds(_index);
}

bool DefaultConstructedVariable::keyPressed(EntityKeyEvent const* _e)
{
	if (VariableNamer::keyPressed(_e))
		return true;
	else if (_e->text() == "=")
	{
		Entity* o = usurp(new AssignedVariable);
		o->prepareChildren();
		o->entity(Identity)->navigateOnto(_e->codeScene());
	}
	else
		return Super::keyPressed(_e);
	return true;
}

bool DefaultConstructedVariable::isInValidState() const
{
	return Super::isInValidState() && actualType()->hasDefaultConstructor();
}

}
