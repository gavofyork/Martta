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

#include <QtXml>

#include "TypeEntity.h"
#include "TextLabel.h"
#include "Variable.h"

namespace Martta
{

MARTTA_OBJECT_CPP(Variable);

Kinds Variable::allowedKinds(int _i) const
{
	if (_i == OurType)
		return Kind::of<TypeEntity>();
	return Super::allowedKinds(_i);
}

void Variable::importDom(QDomElement const& _element)
{
	Super::importDom(_element);

	m_qualifiers = (Qualifiers)_element.attribute("qualifiers").toInt();
}

void Variable::exportDom(QDomElement& _element) const
{
	Super::exportDom(_element);

	_element.setAttribute("qualifiers", m_qualifiers);
}

}
