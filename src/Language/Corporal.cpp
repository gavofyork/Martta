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

#include "EntityKeyEvent.h"
#include "Corporal.h"

namespace Martta
{

MARTTA_INTERFACE_CPP(Corporal);
MARTTA_NAMED_CPP(Corporal, Body);

MARTTA_INTERFACE_CPP(Conditional);
MARTTA_NAMED_CPP(Conditional, Condition);

QString Corporal::defineLayout(ViewKeys&, bool _shrink) const
{
	return (QString(!_shrink || self()->child(Body) && self()->child(Body)->cardinalChildCount() ? ";n;i;%2" : ";%2")).arg(Body);
}

bool Corporal::keyPressed(EntityKeyEvent const* _e)
{
	if ((_e->text() == ")" || _e->text() == "{") && _e->focalIndex() != Body && self()->child(Body))
		self()->child(Body)->navigateOnto(_e->codeScene());
	else
		return false;
	return true;
}

}
