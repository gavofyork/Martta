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

#include "Variable.h"
#include "Construction.h"
#include "Constructor.h"
#include "ConstructedVariable.h"

namespace Martta
{

MARTTA_OBJECT_CPP(ConstructedVariable);	
	
Kinds ConstructedVariable::allowedKinds(int _index) const
{
	switch (_index)
	{
		case 0: return Kind::of<Variable>();
		case 1: return Kind::of<Construction>();
		default: return Kinds();
	}
}

Types ConstructedVariable::allowedTypes(int _index) const
{
	if (_index == 1 && entityIs<Variable>(0))
		return entityAs<TypeNamer>(0)->type();
	return Types();
}

QString ConstructedVariable::code() const
{
	M_ASSERT(entityAs<Construction>(1));
	return entityAs<Variable>(0)->code() + entityAs<Construction>(1)->callList();
}

QString ConstructedVariable::defineLayout(ViewKeys&) const
{
	return "ycode;0;^;'(';1;')'";
}

}
