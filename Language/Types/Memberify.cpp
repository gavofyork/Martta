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

#include "FunctionType.h"
#include "Memberify.h"

namespace Martta
{

MARTTA_PROPER_CPP(Memberify);
MARTTA_NAMED_CPP(Memberify, Scope);

Memberify::Memberify(TypeDefinition* _scope, bool _isConst)
{
	prepareChildren();
	if (child(Scope))
		child(Scope)->killAndDelete();
	middle(Scope).place(new ExplicitType(_scope));
	if (_isConst)
		child(Scope)->insert(new Const);
}

Memberify::Memberify(Type const& _object)
{
	prepareChildren();
	if (child(Scope))
		child(Scope)->killAndDelete();
	_object.placeCopy(middle(Scope));
}

Types Memberify::assignableTypes() const
{
	return Type(*this);
}

bool Memberify::isConst() const
{
	TypeConcept* te = scope();
	if (te && te->isType<Const>())
		return true;
	return false;
}

void Memberify::setConst(bool _c)
{
	// !!!UNTESTED!!!
	TypeConcept* te = scope();
	if (!te)
		return;
	if (!_c && te->isType<Const>())
		te->asType<Const>()->unknit();
	else if (_c && !te->isType<Const>())
		te->insert(new Const);
}

void Memberify::setScope(Type const& _newScope)
{
	AssertNR(scope());
	scope()->replace(TypeConcept::cloneOf(&*_newScope, owner()));
	List<TypeConcept*> l;
	l << childrenOf<TypeConcept>();
	while (!l.isEmpty())
	{
		TypeConcept* t = l.takeLast();
		l << t->childrenOf<TypeConcept>();
		t->rejig();	// Should really be done through Class since it knows about MemberTmplType and Memberify
	}
}

Kinds Memberify::allowedKinds(int _i) const
{
	if (_i == Scope)
		return Kind::of<TypeConcept>();
	return Super::allowedKinds(_i);
}

TypeConcept* Memberify::scopeType() const
{
	if (scope())
		if (scope()->isType<Const>())
			return scope()->asType<Const>()->original();
		else
			return scope();
	else
		return TypeConcept::null;
}

String Memberify::code(String const& _middle) const
{
	if (scopeType())
		return original()->code(" " + scopeType()->code() + ":: " + _middle) + (original()->ignore<ModifyingType>()->isType<FunctionType>() && isConst() ? " const" : "");
	return String::null;
}

bool Memberify::defineSimilarityFrom(TypeConcept const* _f, Castability _c) const
{
	// const and mutable member methods fit in mutable-shaped holes.
	// only const member methods fit in const-shaped holes.
	// (Normal Logical castability test would be mutable->[const|mutable], const->[const]).
	// So... we do a reverse Logical castability test:
	// const->[const|mutable], mutable->[mutable]
	// Similarly, base-class memberified-methods fit into derived-shaped caller holes,
	// but not the other way around.
	return (_f->isKind<Memberify>() && (!scope() || (_f->asKind<Memberify>()->scope() && scope()->isSimilarTo(_f->asKind<Memberify>()->scope(), Physical)))
				&& _f->asKind<Memberify>()->original()->isSimilarTo(original(), _c))
			|| Super::defineSimilarityFrom(_f, _c);
}

String Memberify::modifierHtml() const
{
	if (!scopeType())
		return String::null;
	return L"<span class=\"symbol\">" + scopeType()->code() + L"</span>";
}

}
