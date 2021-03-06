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

#include "Concept.h"
#include "KeyEvent.h"
#include "Corporal.h"

namespace Martta
{

MARTTA_NOTION_CPP(Corporal);
MARTTA_NAMED_CPP(Corporal, Body);

String Corporal::defineHtml(bool) const
{
	return toHtml(child(Body));//, !_shrink || (child(Body) && child(Body)->cardinalChildCount()) ? L"span class=\"block\"" : L"span");
}

bool Corporal::keyPressed(KeyEvent const* _e)
{
	if (_e->text() == ")" && _e->focalIndex() != Body)
		_e->codeScene()->navigateOnto(this);
	else if ((_e->text() == ")" || _e->text() == "{") && _e->focalIndex() != Body && child(Body))
		_e->codeScene()->navigateOnto(child(Body));
	else
		return false;
	return true;
}

}
