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
#include "ReferencedType.h"
#include "Field.h"
#include "ValueDefiner.h"
#include "Compound.h"
#include "Argument.h"
#include "Reference.h"
#include "Const.h"
#include "Constructor.h"

namespace Martta
{

MARTTA_PLACEHOLDER_CPP(ConstructionHelper);

MARTTA_PROPER_CPP(Constructor);
MARTTA_NAMED_CPP(Constructor, Helper);

String Constructor::basicCode(FunctionCodeScope _ref) const
{
	return Martta::code(qualifiers() & FunctionMask) + callingCode(_ref);
}

String Constructor::bodyCode() const
{
	String r;
	foreach (ConstructionHelper* c, childrenOf<ConstructionHelper>(Helper))
		r += c->code();
	return r + Super::bodyCode();
}

bool Constructor::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	return simplePositionKeyPressHandler<Constructor>(_p, _e, "C");
}

String Constructor::nick() const
{
	return typeDefinition()->nick();
}

String Constructor::codeName() const
{
	return typeDefinition()->codeName();
}

Kinds Constructor::allowedKinds(int _i) const
{
	if (_i == Identity || _i == Constness || _i == Returned)
		return Kinds();
	else if (_i == Body)
		return Kind::of<HardCompound>();
	else if (_i == Helper)
		return Kind::of<ConstructionHelper>();
	else if (_i >= 0)
		return Kind::of<Argument>();
	return Super::allowedKinds(_i);
}

bool Constructor::isInValidState() const
{
	if (!Super::isInValidState())
		return false;
	if (argumentCount() == 1)
		return Type(*argumentType(0)->ignore<Const>()) != Type(typeDefinition());
	return true;
}

String Constructor::defineNameHtml() const
{
	return typeDefinition()->labelHtml(typeDefinition()->nick());
}

Type Constructor::returns() const
{
	return Type(typeDefinition()).topWith(Reference());
}

}

