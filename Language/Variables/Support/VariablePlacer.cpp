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

#include "AssignedVariable.h"
#include "DefaultConstructedVariable.h"
#include "TypeConcept.h"
#include "VariablePlacer.h"

namespace Martta
{

bool canPlaceVariable(Position const& _p)
{
	return _p.allowedToBeKind<AssignedVariable>();
}

void placeVariable(Position const& _p, TypeConcept* _t)
{
	Concept* e = 0;
	if (_p.allowedToBeKind<DefaultConstructedVariable>())
		e = new DefaultConstructedVariable;
	else if (_p.allowedToBeKind<AssignedVariable>())
		e = new AssignedVariable;
	if (e)
	{
		e->adopt(_t);
		e->prepareChildren();
		_p.place(e);
		e->child(VariableNamer::OurType)->setCurrent();
	}
	else
		delete _t;
}

}
