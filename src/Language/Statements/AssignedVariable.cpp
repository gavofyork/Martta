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
#include "Variable.h"
#include "DefaultConstructedVariable.h"
#include "AssignedVariable.h"

namespace Martta
{

MARTTA_OBJECT_CPP(AssignedVariable);	
	
Kinds AssignedVariable::allowedKinds(int _index) const
{
	switch (_index)
	{
		case 0: return Kind::of<Variable>();
		case 1: return Kind::of<Typed>();
		default: return Kinds();
	}
}

Types AssignedVariable::allowedTypes(int _index) const
{
	if (_index == 1 && entityAs<Variable>(0))
		return Type(*entityAs<Variable>(0)->actualType());
	return Types();
}

Type AssignedVariable::type() const
{
	return entityIs<Variable>(0) ? entityAs<Variable>(0)->asKind<TypeNamer>()->type() : Type();
}

QString AssignedVariable::code() const
{
	if (!entityAs<Variable>(0)) return "";
	if (!isTyped(1)) return "";
	return entityAs<Variable>(0)->basicCode() + " = " + asTyped(1)->code();
}

bool AssignedVariable::keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e)
{
	if (!_p->isAllowed<DefaultConstructedVariable>())
		return simplePlaceholderKeyPressHandler<AssignedVariable>(_p, _e, "V");
	else
		return false;
}

bool AssignedVariable::keyPressed(EntityKeyEvent const* _e)
{
	if (_e->text() == "=")
		entity(1)->setCurrent();
	else
		return Super::keyPressed(_e);
	return true;
}

QString AssignedVariable::defineLayout(ViewKeys&) const
{
	return "0;ycode;Mi;^;':=';Mi;1";
}

}
