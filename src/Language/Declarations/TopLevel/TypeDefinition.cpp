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
#include "ExplicitType.h"
#include "TypeDefinition.h"

namespace Martta
{

MARTTA_INTERFACE_CPP(TypeDefinition);	

bool TypeDefinition::hasSingleCastOperator(TypeEntity const* _t, bool) const
{
	return Type(*_t) == Type(const_cast<TypeDefinition*>(this));
}

bool TypeDefinition::defineSimilarityTo(TypeEntity const* _t, TypeEntity::Castability _c) const
{
	if (_c == TypeEntity::Convertible && hasSingleCastOperator(_t, false))
		return true;
	return false;
}

List<ValueDefiner*> TypeDefinition::applicableMembers(Entity*, bool) const
{
	return List<ValueDefiner*>();
}

}
