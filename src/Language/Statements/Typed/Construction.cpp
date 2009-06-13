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

#include "Argument.h"
#include "Class.h"
#include "Constructor.h"
#include "ExplicitType.h"
#include "Reference.h"
#include "Const.h"
#include "Construction.h"

namespace Martta
{

MARTTA_OBJECT_CPP(Construction);	
	
int Construction::minimumRequired() const
{
	return m_subject->entities().size() - 2;
}

Kinds Construction::allowedKinds(int _index) const
{
	if (m_subject.isUsable() && _index < m_subject->entities().size() - 2)
		return Kind::of<Typed>();
	else
		return Kinds();
}

Types Construction::allowedTypes(int _index) const
{
	if (!m_subject.isUsable()) return Types();
	
	Type t = *m_subject->entityAs<Argument>(_index + 1)->actualType();
	if (t.isNull())
		return Type(Void).topWith(Const());
	return t;
}

Type Construction::type() const
{
	if (!m_subject.isUsable())
		return Type();
	return Type(m_subject->classType()).topWith(Reference());
}

QString Construction::code() const
{
	if (!m_subject.isUsable())
		return QString();
	return m_subject->classType()->reference() + callList();
}

void Construction::importDom(QDomElement const& _element)
{
	Entity::importDom(_element);
	m_subject = locateEntity<Constructor>(_element.attribute("subject"));
}

void Construction::exportDom(QDomElement& _element) const
{
	Entity::exportDom(_element);
	_element.setAttribute("subject", m_subject->key());
}

}