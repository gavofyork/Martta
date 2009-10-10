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

#include "TypeDefinition.h"
#include "TextLabel.h"
#include "AccessLabel.h"
#include "Member.h"

namespace Martta
{

MARTTA_PLACEHOLDER_CPP(Member);
MARTTA_NAMED_CPP(Member, Accessibility);

Access Member::access() const
{
	return isComplete() ? childAs<AccessLabel>(Accessibility)->access() : NoAccess;
}

TypeDefinition* Member::typeDefinition() const
{
	return parentAs<TypeDefinition>();
}

Kinds Member::allowedKinds(int _i) const
{
	if (_i == Identity)
		return Kind::of<TextLabel>();
	if (_i == Accessibility)
		return Kind::of<AccessLabel>();
	else
		return Super::allowedKinds(_i);
}

String Member::interfaceCode() const
{
	if (!isComplete())
		return String::null;
	return childAs<Label>(Accessibility)->code() + ": " + memberInterfaceCode();
}

String Member::defineHtml() const
{
	return L"<^>" + memberDefineHtml();
}

bool Member::keyPressed(KeyEvent const* _e)
{
	// TODO: exchange for usurps
	if (child(Accessibility)->keyPressed(_e))
		return true;
	return Super::keyPressed(_e);
}

}
