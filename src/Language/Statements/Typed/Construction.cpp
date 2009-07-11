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
#include "Reference.h"
#include "Construction.h"

namespace Martta
{

MARTTA_OBJECT_CPP(Construction);	
	
int Construction::minRequired(int _i) const
{
	if (_i == Cardinals)
		return m_subject->cardinalChildCount() - 2;
	else
		return Super::minRequired(_i);
}

Kinds Construction::allowedKinds(int _index) const
{
	if (m_subject.isUsable() && _index < m_subject->cardinalChildCount() - 2)
		return Kind::of<Typed>();
	else
		return Super::allowedKinds(_index);
}

Types Construction::allowedTypes(int _index) const
{
	if (m_subject.isUsable() && _index >= 0 && _index < m_subject->argumentCount())
	{
		Type t = *m_subject->argumentType(_index);
		if (t.isNull())
			return Types();
		return t;
	}
	return Super::allowedTypes(_index);
}

Type Construction::type() const
{
	if (!m_subject.isUsable())
		return Type();
	return Type(m_subject->typeDefinition()).topWith(Reference());
}

String Construction::code() const
{
	if (!m_subject.isUsable())
		return String();
	return m_subject->parentAs<Identifiable>()->reference() + callList();	// parentAs<>() actually gives the class.
}

}
