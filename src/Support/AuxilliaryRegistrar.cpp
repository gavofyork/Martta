/* ***** BEGIN LICENSE BLOCK *****
 * Version: Martta License version 1.0
 *
 * The contents of this file are subject to the Martta License version
 * 1.0 (the "License"); you may not use this file except in compliance with
 * the License. You should have received a copy of the Martta License
 * "COPYING.Martta" along with Martta; if not you may obtain a copy of the
 * License at http://quidprocode.co.uk/Martta/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Initial Developer of the code in this file is Gavin Wood.
 * Portions created by the Initial Developer are Copyright (C) 2008
 * quid pro code, Ltd. All Rights Reserved.
 *
 * ***** END LICENSE BLOCK ***** */

#include "AuxilliaryRegistrar.h"

namespace Martta
{

AuxilliaryRegistrar* AuxilliaryRegistrar::s_this = 0;

void AuxilliaryRegistrar::initialiseAll() const
{
	foreach (AuxilliaryFace const* i, m_auxilliaries.values())
		i->initialise();
	
	recurseAux(m_derivedMap.values(0)[0], "");
}
	
void AuxilliaryRegistrar::recurseAux(AuxilliaryFace const* _face, QString const& _indent) const
{
	qInformation() << _indent << _face->name();
	foreach (AuxilliaryFace const* i, m_derivedMap.values(_face))
		recurseAux(i, _indent + "|   ");
}

void AuxilliaryRegistrar::registerAuxilliary(AuxilliaryFace const* _a)
{
	qInformation() << "Registering" << _a->name();
	m_auxilliaries[_a->name()] = _a;
	m_derivedMap.insert(_a->superAuxilliary(), _a);
}
	
}
