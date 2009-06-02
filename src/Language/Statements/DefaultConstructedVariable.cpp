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

// GAA Don't really want to have to care about this, but necessary at the moment since MemberVariable inherits from Variable and we need to 
// be able to accept it generally. In reality, MemberVariable shouldn't inherit from Variable - just contain it or something, or perhaps
// have BaseVariable -> { Variable, MemberVariable } 
#include "MemberVariable.h"

#include "Variable.h"
#include "AssignedVariable.h"
#include "DefaultConstructedVariable.h"

namespace Martta
{

MARTTA_OBJECT_CPP(DefaultConstructedVariable);	
	
Kinds DefaultConstructedVariable::allowedKinds(int _index) const
{
	switch (_index)
	{
		case 0: return Kind::of<Variable>();
		default: return Kinds();
	}
}

Kinds DefaultConstructedVariable::deniedKinds(int _index) const
{
	switch (_index)
	{
		case 0: return Kind::of<MemberVariable>();
		default: return Kinds();
	}
}

bool DefaultConstructedVariable::isInValidState() const
{
	if (!Super::isInValidState())
		return false;
	return entityAs<Variable>(0)->actualType()->hasDefaultConstructor();
}

QString DefaultConstructedVariable::code() const
{
	M_ASSERT(entityIs<Variable>(0));
	return entityAs<Variable>(0)->code();
}

bool DefaultConstructedVariable::keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e)
{
	return simplePlaceholderKeyPressHandler<DefaultConstructedVariable>(_p, _e, "V");
}

QString DefaultConstructedVariable::defineLayout(ViewKeys&) const
{
	return "^;0";
}

bool DefaultConstructedVariable::keyPressed(EntityKeyEvent const* _e)
{
	if (_e->text() == "=")
	{
		Entity* o = usurp(new AssignedVariable);
		o->prepareChildren();
		o->entity(1)->navigateOnto(_e->codeScene());
	}
	else
		return Super::keyPressed(_e);
	return true;
}

}
