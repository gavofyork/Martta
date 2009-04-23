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

#include "FunctionType.h"
#include "Reference.h"
#include "Compound.h"
#include "TypeEntity.h"
#include "Variable.h"
#include "TextLabel.h"
#include "ThisPointer.h"
#include "Memberify.h"
#include "Const.h"
#include "Reference.h"
#include "Callable.h"
#include "CodeScene.h"

namespace Martta
{

MARTTA_OBJECT_CPP(Callable);	

QString Callable::defineLayout(ViewKeys& _viewKeys) const
{
	return "^;2;Mi;>name;ynormal;s" + FunctionType().idColour() + ";!0;ycode;'(';" + times(firstArgumentIndex(), entityCount(), ";', ';") + ";')';#EOA" + QString(_viewKeys["expanded"].toBool() ? body()->entities().size() ? ";n;i;1" : ";1" : "");
}

QString Callable::implementationCode() const
{
	return code(Callable::OutsideScope) + "\n" + (body() ? body()->code() : QString());
}

Variable* Callable::argument(int _index) const
{
	M_ASSERT(_index < argumentCount());
	if (!entityIs<Variable>(_index + firstArgumentIndex()))
	{
		debugTree();
		M_ASSERT(false);
	}
	return entityAs<Variable>(_index + firstArgumentIndex());
}

QString Callable::identity() const
{
	return type()->code(name()).replace(" ", "").replace("::", ";;");
}

Type Callable::type() const
{
	Type ret = FunctionType(this->ellipsis());
	ret.append(returns());
	for (int i = 0; i < argumentCount(); i++)
		ret.append(argumentType(i));

	ret.topWith(Reference());
	return ret;
}

QString Callable::callingCode(FunctionCodeScope _ref) const
{
	QString ret = ((_ref == OutsideScope) ? reference() : codeName()) + "(";
	for (int i = 0; i < argumentCount(); i++)
	{
		if (i) ret += ", ";
			ret += argumentType(i)->code(" " + argumentCodeName(i));
	}
	if (ellipsis())// && _ref == OutsideScope)
		ret += argumentCount() ? ", ..." : "...";
	ret += ")";
	return ret;
}

QString Callable::code(FunctionCodeScope _ref) const
{
	return Martta::code(qualifiers() & MethodMask) + returns()->code(" " + callingCode(_ref));
}

Kinds Callable::allowedKinds(int _i) const
{
	if (_i == 0)
		return Kind::of<TextLabel>();
	if (_i == 1)
		return Kind::of<Compound>();
	if (_i == 2)
		return Kind::of<TypeEntity>();
	if (_i >= firstArgumentIndex())
		return Kind::of<Variable>();
	return Kinds();
}

Kinds Callable::deniedKinds(int) const
{
	return Kind::of<ThisPointer>();
}

bool Callable::activated(CodeScene* _s)
{
	_s->viewKeys(this)["expanded"] = !(_s->viewKeys(this)["expanded"].toBool());
	relayout(_s);
	
	if (_s->viewKeys(this)["expanded"].toBool())
		body()->entity(0)->setCurrent();
	else
		setCurrent();
	return true;
}

bool Callable::keyPressed(EntityKeyEvent const* _e)
{
	if (_e->text() == "(" && !argumentCount() && (_e->focalIndex() == 0 || _e->isFocused()) || _e->text() == "," && _e->focalIndex() >= firstArgumentIndex())
	{
		Variable* v = new Variable;
		back().place(v);
		v->prepareChildren();
		v->navigateInto(_e->codeScene());
	}
	else if (_e->text() == "(" && _e->focalIndex() == 0)
	{
		argument(0)->navigateInto(_e->codeScene());
	}
	else if (_e->text() == "{")
	{
		_e->codeScene()->viewKeys(this)["expanded"] = true;
		_e->codeScene()->relayoutLater(this);
		_e->codeScene()->setCurrent(body()->entity(0));
	}
	else if (_e->text() == "}")
	{
		_e->codeScene()->viewKeys(this)["expanded"] = false;
		_e->codeScene()->relayoutLater(this);
		setCurrent();
	}
	else if (_e->text() == " " && entityIs<TypeEntity>(_e->focalIndex()))
	{
		entity(0)->navigateOnto(_e->codeScene());
	}
	else
		return Super::keyPressed(_e);
	return true;
}

void Callable::importDom(QDomElement const& _element)
{
	Super::importDom(_element);
	m_qualifiers = (Qualifiers)_element.attribute("qualifiers").toInt();
}

void Callable::exportDom(QDomElement& _element) const
{
	Super::exportDom(_element);
	_element.setAttribute("qualifiers", m_qualifiers);
}

}
