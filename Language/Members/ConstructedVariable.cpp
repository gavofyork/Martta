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
#include "TypeConcept.h"
#include "Construction.h"
#include "ConstructedVariable.h"

namespace Martta
{

MARTTA_PROPER_CPP(ConstructedVariable);
MARTTA_NAMED_CPP(ConstructedVariable, OurConstruction);

Kinds ConstructedVariable::allowedKinds(int _index) const
{
	if (_index == OurType)
		return Kind::of<TypeConcept>();
	else if (_index == OurConstruction)
		return Kind::of<Construction>();
	else if (_index == Identity)
		return Kind::of<TextLabel>();
	return Super::allowedKinds(_index);
}

String ConstructedVariable::code() const
{
	return basicCode() + childAs<Construction>(OurConstruction)->callList();
}

bool ConstructedVariable::keyPressed(KeyEvent const* _e)
{
	if (VariableNamer::keyPressed(_e))
		return true;
	else if (_e->text() == "(")
		_e->codeScene()->setCurrent(child(OurConstruction));
	else
		return Super::keyPressed(_e);
	return true;
}

}
