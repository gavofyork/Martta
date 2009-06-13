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

#include "Reference.h"
#include "IdLabel.h"
#include "VariableNamer.h"

namespace Martta
{

MARTTA_INTERFACE_CPP(VariableNamer);

TypeEntity* VariableNamer::actualType() const
{
	if (self()->entityCountOf<TypeEntity>() == 1)
		return self()->entitiesOf<TypeEntity>()[0];
	return TypeEntity::null;
}

QString VariableNamer::basicCode() const
{
	if (self()->entityCountOf<IdLabel>() == 1)
		return actualType()->code(" " + self()->entitiesOf<IdLabel>()[0]->code());
	else
		return QString::null;
}

bool VariableNamer::keyPressed(EntityKeyEvent const* _e)
{
	if (_e->text() == " " && _e->focalIndex() == 1)
	{
		self()->entity(0)->navigateOnto(_e->codeScene());
	}
	else
		return false;
	return true;
}

Type VariableNamer::type() const
{
	if (self()->entityCountOf<TypeEntity>() == 1)
	{
		Type ret(*self()->entitiesOf<TypeEntity>()[0]);
		if (!ret->isType<Reference>())
			ret.topWith(Reference());
		return ret;
	}
	else
		return Type();
}

QString VariableNamer::defineLayout(ViewKeys&) const
{
	return "1;s" + actualType()->idColour() + ";Mi;>name;fb0;!0";
}

}
