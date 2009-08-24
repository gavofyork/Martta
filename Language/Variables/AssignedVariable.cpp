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
#include "Typed.h"
#include "DefaultConstructedVariable.h"
#include "AssignedVariable.h"

namespace Martta
{

MARTTA_OBJECT_CPP(AssignedVariable);

Kinds AssignedVariable::allowedKinds(int _index) const
{
	if (_index == OurType)
		return Kind::of<TypeEntity>();
	else if (_index == AssignedValue)
		return Kind::of<Typed>();
	else if (_index == Identity)
		return Kind::of<TextLabel>();
	return Super::allowedKinds(_index);
}

Types AssignedVariable::allowedTypes(int _index) const
{
	if (_index == AssignedValue)
		return Type(*actualType());
	return Super::allowedTypes(_index);
}

String AssignedVariable::code() const
{
	return basicCode() + " = " + asTyped(AssignedValue)->code();
}

bool AssignedVariable::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	if (!_p.allowedToBeKind<DefaultConstructedVariable>())
		return simplePlaceholderKeyPressHandler<AssignedVariable>(_p, _e, "V");
	else
		return false;
}

void AssignedVariable::onDependencyChanged(int, Entity*)
{
	changed(Logically);
}

void AssignedVariable::onDependencySwitched(Entity* _e, Entity*)
{
//	mDebug() << this << ": Dependency Switched: " << _e;
//	debugTree();
	if (_e == child(AssignedValue) && _e->kind() == Kind::of<Typed>())
	{
		Entity* o = usurp(new DefaultConstructedVariable);
		_e->killAndDelete();
		o->setCurrent();
	}
}

bool AssignedVariable::keyPressed(KeyEvent const* _e)
{
	if (VariableNamer::keyPressed(_e))
		return true;
	else if (_e->text() == "=")
		child(AssignedValue)->setCurrent();
	else
		return Super::keyPressed(_e);
	return true;
}

}
