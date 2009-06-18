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

#include "Reference.h"
#include "FunctionType.h"
#include "Pointer.h"
#include "SimpleType.h"
#include "Memberify.h"
#include "Const.h"
#include "Invocation.h"

namespace Martta
{

MARTTA_OBJECT_CPP(Invocation);	
	
Type Invocation::type() const
{
	if (isTyped(0) && typeOf(0)->isType<FunctionType>())
		return typeOf(0)->asType<FunctionType>()->returnType();
	return Type();
	// TODO: Handle objects with () operators.
}

QString Invocation::code() const
{
	return isTyped(0) ? asTyped(0)->code() + callList(typeds().mid(1)) : "";
}

QString Invocation::callList(QList<Typed*> _parameters) const
{
	QString ret;
	ret += "(";
	bool first = true;
	foreach (Typed* s, _parameters)
	{
		if (!first) ret += ", ";
		first = false;
		ret += s->code();
	}
	ret += ")";
	return ret;
}

int Invocation::minimumRequired() const
{
	if (typeOf(0)->isType<FunctionType>())
		return typeOf(0)->asType<FunctionType>()->minimumArgCount() + 1;
	return 1;
}

Kinds Invocation::allowedKinds(int _index) const
{
	if (_index == 0)
		return Kind::of<Typed>();
	else if (typeOf(0)->isType<FunctionType>()
		&& typeOf(0)->asType<FunctionType>()->hasArgumentAt(_index - 1))
		return Kind::of<Typed>();
	else
		return Super::allowedKinds(_index);
}

Types Invocation::allowedTypes(int _index) const
{
	if (_index == 0)
	{
		return Type(FunctionType(false, true));
	}
	else if (typeOf(0)->isType<FunctionType>() && typeOf(0)->asType<FunctionType>()->hasArgumentAt(_index - 1))
	{
		return typeOf(0)->asType<FunctionType>()->argumentType(_index - 1);
	}
	return Types();
}

QString Invocation::defineLayout(ViewKeys&) const
{
	QString ret = "0;ycode;'(';^";
	for (int i = 1; i < cardinalChildCount(); i++)
		ret += QString((i == 1) ? ";%1" : ";', ';%1").arg(i);
	return ret + ";')'";
}

void Invocation::onDependencyChanged(Entity* _e)
{
	if (_e == child(0))
	{
		// The function we are calling has changed.
		validifyChildren();
//		changed();
	}
	Super::onDependencyChanged(_e);
}

bool Invocation::keyPressed(EntityKeyEvent const* _e)
{
	if (_e->text() == "(" && _e->focalIndex() == 0 && child(1))
		child(1)->navigateOnto(_e->codeScene());
	else if (_e->text() == "(" && _e->focalIndex() == 0)
		setCurrent();
	else if (_e->text() == ")" && !_e->isFocused())
		setCurrent();
	else if (_e->text() == "," && _e->focalIndex() > 0 && _e->focalIndex() < cardinalChildCount() - 1)
		// Jump to next if we press a comma in the parameter list, before the last item.
		child(_e->focalIndex() + 1)->setCurrent();
	else if (_e->text() == "," && back().allowedKinds().size())
		// Insert a new item if we press a comma anywhere else, if we're allowed to.
		back().spawnPrepared()->setCurrent();
	else
		return Super::keyPressed(_e);
	return true;
}

bool Invocation::keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e)
{
	if (_p.exists() && !_p->isPlaceholder() && _p->isKind<Typed>() && _e->text() == "(" &&
		_p->asKind<Typed>()->type()->isType<FunctionType>() && !(_p->contextIs<Invocation>() && _p->contextIndex() == 0) && !isTemporary(_p.childType()))
	{
		Entity* n = new Invocation;
		_p->insert(n);
		n->prepareChildren();
		if (n->cardinalChildCount() > 1)
			n->dropCursor();
		else
		{
			n->relayout();
			n->setCurrent();
		}
		return true;
	}
	return false;
}

}
