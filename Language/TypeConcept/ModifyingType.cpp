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

#include "Type.h"
#include "ModifyingType.h"

namespace Martta
{

MARTTA_PLACEHOLDER_CPP(ModifyingType);
const int& ModifyingType::Original = ModifyingType::Default;

void ModifyingType::unknit()
{
	AssertNR(original());
	if (owner())
	{
		AssertNR(original()->owner() == owner());
		if (&**owner() == this)
		{
			AssertNR(!parent());
			owner()->m_top = original();
		}
	}
	//	P -i-> this -0-> original    BECOMES    P -i-> original
	replace(original());
}

Kinds ModifyingType::allowedKinds(int _index) const
{
	if (_index == Original)
		return Kind::of<TypeConcept>();
	return Super::allowedKinds(_index);
}

}
