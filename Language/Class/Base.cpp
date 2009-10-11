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
#include "ReferencedType.h"
#include "Base.h"

namespace Martta
{

MARTTA_PROPER_CPP(Base);
MARTTA_NAMED_CPP(Base, Accessibility);
MARTTA_NAMED_CPP(Base, Superclass);

Class* Base::classType() const
{
	if (!isComplete() || !childAs<ReferencedType>(Superclass)->subject()->isKind<Class>())
		return 0;
	return childAs<ReferencedType>(Superclass)->subject()->asKind<Class>();
}

bool Base::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	return simplePositionKeyPressHandler<Base>(_p, _e, "B");
}

void Base::onDependencyChanged(int, Concept*)
{
	changed(Logically);
}

bool Base::isChildInValidState(int _i) const
{
	if (childIs<ReferencedType>(_i))
		return Super::isChildInValidState(_i) && childAs<ReferencedType>(_i)->subject()->isKind<Class>() && childAs<ReferencedType>(_i)->subject()->self() != parent();
	return Super::isChildInValidState(_i);
}

Access Base::access() const
{
	if (!isComplete())
		return NoAccess;
	return childAs<AccessLabel>(Accessibility)->access();
}

String Base::code() const
{
	if (!isComplete())
		return String::null;
	return childAs<Label>(Accessibility)->code() + " " + childAs<TypeConcept>(Superclass)->code();
}

bool Base::keyPressed(KeyEvent const* _e)
{
	if (child(Accessibility) && child(Accessibility)->keyPressed(_e))		//QUICK do with usurp
		return true;
	return Super::keyPressed(_e);
}

Kinds Base::allowedKinds(int _i) const
{
	if (_i == Accessibility)
		return Kind::of<AccessLabel>();
	if (_i == Superclass)
		return Kind::of<ReferencedType>();
	return Super::allowedKinds(_i);
}

String Base::defineHtml() const
{
	return L"<^><span class=\"keyword\">inherits</span> " + toHtml(child(Superclass)) + L" <span class=\"keyword\">as</span> "+ toHtml(child(Accessibility));
}

}
