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

#include "TextLabel.h"
#include "TypeConcept.h"
#include "IdentifierSet.h"
#include "Typed.h"
#include "DefaultConstructedVariable.h"
#include "WebStylist.h"
#include "AssignedVariable.h"

namespace Martta
{

MARTTA_PROPER_CPP(AssignedVariable);
MARTTA_NAMED_CPP(AssignedVariable, AssignedValue);
MARTTA_NOTION_CPP(Declarable);

Kinds AssignedVariable::allowedKinds(int _index) const
{
	if (_index == OurType)
		return Kind::of<TypeConcept>();
	else if (_index == AssignedValue)
		return Kind::of<Typed>();
	else if (_index == Identity)
		return Kind::of<TextLabel>();
	return Super::allowedKinds(_index);
}

void AssignedVariable::onDependencyChanged(int _a, Concept* _e)
{
	if (childIs<Declarable>(OurType) && _e == tryChild<Typed>(AssignedValue) && _a == Logically)
		changed(Logically);
	VariableNamer::onDependencyChanged(_a, _e);
	Super::onDependencyChanged(_a, _e);
}

Type AssignedVariable::actualType() const
{
	if (TypeConcept* t = tryChild<TypeConcept>(OurType))
	{
		if (Declarable* d = t->tryKind<Declarable>())
			if (Typed* t = tryChild<Typed>(AssignedValue))
				return d->declaredType(*t->bareType());
		if (!t->isNull())
			return *t;
	}
	return Type();
}

Types AssignedVariable::allowedTypes(int _index) const
{
	if (_index == AssignedValue)
		return actualType();
	return Super::allowedTypes(_index);
}

String AssignedVariable::code() const
{
	return basicCode() + " = " + asTyped(AssignedValue)->code();
}

bool AssignedVariable::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	if (!_p.allowedToBeKind<DefaultConstructedVariable>())
		return simplePositionKeyPressHandler<AssignedVariable>(_p, _e, "V");
	else
		return false;
}

void AssignedVariable::onDependencySwitched(Concept* _e, Concept*)
{
//	mDebug() << this << ": Dependency Switched: " << _e;
//	debugTree();
	if (_e == child(AssignedValue) && _e->kind() == Kind::of<Typed>())
	{
		usurp(new DefaultConstructedVariable);
		_e->killAndDelete();
	}
}

String AssignedVariable::defineHtml() const
{
	String m = WebStylist::current()->property("CSS", "Simple").toBool() ? L"=" : L":=";
	return defineVariableHtml() + L"<^><span class=\"symbol\"> " + m + L" </span>" + toHtml(child(AssignedValue));
}

bool AssignedVariable::keyPressed(KeyEvent const* _e)
{
	if (VariableNamer::keyPressed(_e))
		return true;
	else if (_e->text() == "=")
		_e->codeScene()->setCurrent(child(AssignedValue));
	else
		return Super::keyPressed(_e);
	return true;
}

}
