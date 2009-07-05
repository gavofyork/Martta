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

#include "Kind.h"
#include "Compound.h"
#include "AccessLabel.h"
#include "ConstLabel.h"
#include "ConversionOperator.h"

namespace Martta
{

MARTTA_OBJECT_CPP(ConversionOperator);

String ConversionOperator::basicCode(FunctionCodeScope _ref) const
{
	return Martta::code(qualifiers() & MethodMask) + callingCode(_ref) + (isConst() ? " const" : "");
}

bool ConversionOperator::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	return simplePositionKeyPressHandler<ConversionOperator>(_p, _e, "X");
}

String ConversionOperator::defineNameLayout(ViewKeys const&) const
{
	return String("ycode;'operator';Mi;1");
}

String ConversionOperator::name() const
{
	return "_operator" + returns()->code();
}

String ConversionOperator::codeName() const
{
	return "operator " + returns()->code();
}

Kinds ConversionOperator::allowedKinds(int _i) const
{
	if (_i == Identity)
		return Kinds();
	return Super::allowedKinds(_i);
}

}
