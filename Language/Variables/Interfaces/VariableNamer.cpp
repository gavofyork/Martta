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

MARTTA_NOTION_CPP(VariableNamer);
MARTTA_NAMED_CPP(VariableNamer, OurType);

Type VariableNamer::actualType() const
{
	if (TypeConcept* t = tryChild<TypeConcept>(OurType))
		return *t;
	return Type();
}

String VariableNamer::basicCode() const
{
	if (IdLabel* i = tryChild<IdLabel>(Identity))
		return actualType()->code(" " + i->code());
	else
		return String::null;
}

bool VariableNamer::keyPressed(KeyEvent const* _e)
{
	if (_e->text() == L" " && _e->focalIndex() == OurType && !child(OurType)->isPlaceholder())
	{
		_e->codeScene()->navigateOnto(child(Identity));
	}
	else
		return false;
	return true;
}

Type VariableNamer::type() const
{
	if (!actualType()->isNull())
	{
		Type ret((actualType()));
		if (!ret->isType<Reference>())
			ret.topWith(Reference());
		return ret;
	}
	else
		return Type();
}

String VariableNamer::defineVariableHtml() const
{
	return toHtml(child(OurType)) + L" " + actualType()->typeHtml(toHtml(child(Identity)));
}

}
