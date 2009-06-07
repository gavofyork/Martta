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

#include "Common.h"
#include "Referenced.h"
#include "Reference.h"
#include "Memberify.h"
#include "TextLabel.h"
#include "Callable.h"
#include "Variable.h"

// !TAKE ME OUT
#include "ExplicitType.h"

namespace Martta
{

MARTTA_OBJECT_CPP(Variable);

TypeEntity* Variable::actualType() const
{
	if (!entityIs<TypeEntity>(1))
		return TypeEntity::null;
	return entityAs<TypeEntity>(1);
}

Type Variable::type() const
{
	if (!entityIs<TypeEntity>(1))
		return Type();

	Type ret(*entityAs<TypeEntity>(1));

	if (!ret->isType<Reference>())
		ret.topWith(Reference());
	return ret;
}

bool Variable::isChildInValidState(int _i) const
{
	return _i != 1 || !entityIs<TypeEntity>(1) || Type(*entityAs<TypeEntity>(1)) != Type(Void);
}

Kinds Variable::allowedKinds(int _i) const
{
	if (_i == 0)
		return Kind::of<TextLabel>();
	if (_i == 1)
		return Kind::of<TypeEntity>();
	return Kinds();
}

QString Variable::code() const
{
	if (!isComplete())
		return QString();
	return Martta::code(m_qualifiers & VariableMask) + actualType()->code(" " + entityAs<Label>(0)->code());
}

QString Variable::interfaceCode() const
{
	return code() + ";\n";
}

QString Variable::implementationCode() const
{
	return QString();
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

QString Variable::defineLayout(ViewKeys&) const
{
	return QString(contextIs<Callable>() ? "^;" : "") + "1;s" + entityAs<TypeEntity>(1)->idColour() + ";Mi;>name;fb0;!0";
}

QList<DeclarationEntity*> Variable::utilised() const
{
	return actualType()->utilised();
}

bool Variable::keyPressed(EntityKeyEvent const* _e)
{
	if (_e->text() == " " && _e->focalIndex() == 1)
	{
		entity(0)->navigateOnto(_e->codeScene());
	}
	else if (_e->key() == Qt::Key_Delete && contextIs<Statement>())
	{
		_e->reinterpretLater();
		contextAs<Statement>()->setCurrent();
	}
	else
		return Super::keyPressed(_e);
	return true;
}

}
