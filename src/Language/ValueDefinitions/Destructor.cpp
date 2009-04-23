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

#include "ExplicitType.h"
#include "Class.h"
#include "Compound.h"
#include "Variable.h"
#include "Reference.h"
#include "AccessLabel.h"
#include "Destructor.h"

namespace Martta
{

MARTTA_OBJECT_CPP(Destructor);	
	
QString Destructor::code(FunctionCodeScope _ref) const
{
	return Martta::code(qualifiers() & MethodMask) + ((_ref == InsideScope) ? reference() : codeName()) + "()";
}
	
bool Destructor::keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e)
{
	return simpleInsertionPointKeyPressHandler<Destructor>(_p, _e, "~");
}

Kinds Destructor::allowedKinds(int _i) const
{
	if (_i == 0)
		return Kind::of<Compound>();
	else if (_i == 1)
		return Kind::of<AccessLabel>();
	else
		return Kinds();
}

QString Destructor::defineMemberLayout(ViewKeys& _viewKeys) const
{
	return "^;>name;ycode;'" + name() + "()'" + QString(_viewKeys["expanded"].toBool() ? body()->entities().size() ? ";n;i;0" : ";0" : "");
}

QString Destructor::name() const
{
	return "~" + classType()->name();
}

QString Destructor::codeName() const
{
	return "~" + classType()->codeName();
}

Class* Destructor::classType() const
{
	return contextAs<Class>();
}

}
