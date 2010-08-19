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
#include "BuiltinType.h"
#include "Memberify.h"
#include "Const.h"
#include "Invocation.h"

namespace Martta
{

MARTTA_PROPER_CPP(Invocation);
MARTTA_NAMED_CPP(Invocation, Callee);

Type Invocation::calleeType() const
{
	if (isTyped(Callee))
	{
		if (typeOf(Callee)->isType<FunctionType>())
			return typeOf(Callee);
		if (typeOf(Callee)->isType<ReferencedType>())
			if (Type t = typeOf(Callee)->asType<ReferencedType>()->isCallable())
				return t;
	}
	return Type();
}

Type Invocation::type() const
{
	if (Type t = calleeType())
		return t->asType<FunctionType>()->returnType();
	return Type();
}

String Invocation::code() const
{
//	mDebug() << this << cardinalChildrenAs<Typed>().size() << callList(cardinalChildrenAs<Typed>());
//	debugTree();
	return isTyped(Callee) ? asTyped(Callee)->code() + callList(cardinalChildrenAs<Typed>()) : "";
}

String Invocation::callList(List<Typed*> _parameters) const
{
	String ret;
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

int Invocation::minRequired(int _i) const
{
	if (_i == Callee)
		return 1;
	if (_i == Cardinals)
		if (Type t = calleeType())
			return t->asType<FunctionType>()->minimumArgCount();
		else
			return 0;
	else
		return Super::minRequired(_i);
}

Kinds Invocation::allowedKinds(int _index) const
{
	Type t;
	if (_index == Callee || (_index >= 0 && (t = calleeType()) && t->asType<FunctionType>()->hasArgumentAt(_index)))
		return Kind::of<Typed>();
	return Super::allowedKinds(_index);
}

Types Invocation::allowedTypes(int _index) const
{
	if (_index == Callee)
		return Type(FunctionType(false, true));
	Type t;
	if (_index >= 0 && (t = calleeType()) && t->asType<FunctionType>()->hasArgumentAt(_index))
		return t->asType<FunctionType>()->argumentType(_index);
	return Super::allowedTypes(_index);
}

String Invocation::defineHtml() const
{
	return toHtml(child(Callee)) + mark() + tagOf(L"minor symbol", L"(") + toHtml(cardinalChildren(), L", ") + tagOf(L"minor symbol", L")");
}

void Invocation::onDependencyChanged(int _a, Concept* _e)
{
	if (_e == child(Callee) && !child(Callee)->isPlaceholder() && child(Callee)->isInValidState())
	{
		// The function we are calling has changed.
		validifyChildren();
		changed(Logically);
	}
	Super::onDependencyChanged(_a, _e);
}

bool Invocation::keyPressed(KeyEvent const* _e)
{
	if (_e->text() == "(" && _e->focalIndex() == Callee && child(0))
		_e->codeScene()->navigateOnto(child(0));
	else if (_e->text() == "(" && _e->focalIndex() == Callee)
		_e->codeScene()->setCurrent(this);
	else if (_e->text() == ")" && !_e->isFocused())
		_e->codeScene()->setCurrent(this);
	else if (_e->text() == "," && _e->focalIndex() >= 0 && _e->focalIndex() < cardinalChildCount() - 1)
		_e->codeScene()->setCurrent(child(_e->focalIndex() + 1));
	else if (_e->text() == "," && (_e->focalIndex() >= 0 || _e->focalIndex() == UndefinedIndex) && back().allowedKinds().size())
	{
		// Jump to next if we press a comma in the parameter list, before the last item.
		Typed* t = new Typed;
		(_e->focalIndex() == UndefinedIndex ? self()->back() : self()->middle(_e->focalIndex() + (_e->isInserting() ? 0 : 1))).place(t);
		_e->codeScene()->setCurrent(t);
	}
	else if (_e->text() == "," && back().allowedKinds().size())
		// Insert a new item if we press a comma anywhere else, if we're allowed to.
		_e->codeScene()->setCurrent(back().spawnPrepared());
	else
		return Super::keyPressed(_e);
	return true;
}

bool Invocation::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	if (_p.exists() && !_p->isPlaceholder() && _p->isKind<Typed>() && _e->text() == "(" &&
		_p->asKind<Typed>()->apparentType()->isType<FunctionType>() && !(_p->parentIs<Invocation>() && _p->index() == Callee) && !isTemporary(_p.concept()))
	{
		Concept* n = new Invocation;
		_p->insert(n, Callee);
		n->prepareChildren();
		_e->codeScene()->setCurrent(n->cardinalChildCount() ? n->child(0) : n);
		return true;
	}
	return false;
}

}
