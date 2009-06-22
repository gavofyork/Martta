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

#include "AddressType.h"
#include "ExplicitType.h"
#include "SimpleType.h"
#include "FunctionType.h"
#include "Memberify.h"
#include "ModifyingType.h"

namespace Martta
{

MARTTA_OBJECT_CPP(ModifyingType);

void ModifyingType::unknit()
{
	if (owner() && &**owner() == this)
	{
		M_ASSERT(original()->owner() == owner());
		M_ASSERT(!parent());
		owner()->m_top = original();
	}
	InsertionPoint p = over();
	TypeEntity* ch = original();
	
	//	P -p-> this -0-> ch    BECOMES    P -p-> ch
	
	p.insertSilent(ch);
	kill();
	
	if (p.parent())
		p.parent()->childSwitched(ch, this);
	ch->contextSwitched(this);
	
	delete this;
}

Kinds ModifyingType::allowedKinds(int _index) const
{
	if (_index == Original)
		return Kind::of<TypeEntity>();
	return Super::allowedKinds(_index);
}

}
