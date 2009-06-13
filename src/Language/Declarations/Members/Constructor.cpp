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
#include "ExplicitType.h"
#include "Class.h"
#include "Compound.h"
#include "Argument.h"
#include "Reference.h"
#include "Const.h"
#include "Constructor.h"

namespace Martta
{

MARTTA_OBJECT_CPP(Constructor);	

QString Constructor::basicCode(FunctionCodeScope _ref) const
{
	return Martta::code(qualifiers() & MethodMask) + callingCode(_ref);
}

bool Constructor::keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e)
{
	return simpleInsertionPointKeyPressHandler<Constructor>(_p, _e, "C");
}

QString Constructor::name() const
{
	return classType()->name();
}

QString Constructor::codeName() const
{
	return classType()->codeName();
}

Kinds Constructor::memberAllowedKinds(int _i) const
{
	if (_i == 0)
		return Kind::of<Compound>();
	else
		return Kind::of<Argument>();
}

bool Constructor::isInValidState() const
{
	if (!Super::isInValidState())
		return false;
	if (argumentCount() == 1)
		return Type(*argumentType(0)->ignore<Const>()) != Type(classType());
	return true;
}

Type Constructor::returns() const
{
	return Type(classType()).topWith(Reference());
}

}

