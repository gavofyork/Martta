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
#include "ConstLabel.h"
#include "ConversionOperator.h"

namespace Martta
{

MARTTA_OBJECT_CPP(ConversionOperator);

QString ConversionOperator::code(FunctionCodeScope _ref) const
{
	return Martta::code(qualifiers() & MethodMask) + callingCode(_ref) + (isConst() ? " const" : "");
}

bool ConversionOperator::keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e)
{
	return simpleInsertionPointKeyPressHandler<ConversionOperator>(_p, _e, "X");
}

QString ConversionOperator::defineMemberLayout(ViewKeys& _viewKeys) const
{
	return "^;>name;ycode;'operator';1;'()';Mo;3;Mo" + QString(_viewKeys["expanded"].toBool() ? body()->entities().size() ? ";n;i;0" : ";0" : "");
}

QString ConversionOperator::name() const
{
	return "_operator" + returns()->code();
}

QString ConversionOperator::codeName() const
{
	return "operator " + returns()->code();
}

Kinds ConversionOperator::allowedKinds(int _i) const
{
	if (_i == 0)
		return Kind::of<Compound>();
	if (_i == 1)
		return Kind::of<TypeEntity>();
	if (_i == 2)
		return Kind::of<AccessLabel>();
	if (_i == 3)
		return Kind::of<ConstLabel>();
	return Kinds();
}

}
