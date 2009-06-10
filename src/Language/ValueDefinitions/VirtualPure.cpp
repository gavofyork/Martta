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

#include "AccessLabel.h"
#include "TextLabel.h"
#include "ConstLabel.h"
#include "VirtualPure.h"

namespace Martta
{

MARTTA_OBJECT_CPP(VirtualPure);

QString VirtualPure::interfaceCode() const
{
	return entitiesOf<AccessLabel>()[0]->asKind<Label>()->code() + ": virtual " + basicCode(LambdaNamer::InsideScope) + " = 0;\n";
}

bool VirtualPure::keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e)
{
	return simpleInsertionPointKeyPressHandler<VirtualPure>(_p, _e, "0");
}
	
Kinds VirtualPure::allowedKinds(int _i) const
{
	if (_i == 0)
		return Kind::of<TextLabel>();
	else if (_i == 1)
		return Kind::of<TypeEntity>();
	else if (_i == 2)
		return Kind::of<AccessLabel>();
	else if (_i == 3)
		return Kind::of<ConstLabel>();
	else
		return Kind::of<Variable>();
}
	
}
