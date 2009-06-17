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

#include "Class.h"
#include "Const.h"
#include "FunctionType.h"
#include "ExplicitType.h"
#include "Type.h"
#include "MemberTemplateType.h"
#include "Memberify.h"

namespace Martta
{

MARTTA_OBJECT_CPP(Memberify);

Memberify::Memberify(Type const& _object)
{
	prepareChildren();
	if (entity(1))
		entity(1)->killAndDelete();
	_object.placeCopy(middle(1));
}

Types Memberify::assignableTypes() const
{
	return Type(*this);
}

bool Memberify::isConst() const
{
	TypeEntity* te = entity(1) ? entity(1)->asKind<TypeEntity>() : 0;
	if (te && te->isType<Const>())
		return true;
	return false;
}

void Memberify::setConst(bool _c)
{
	// !!!UNTESTED!!!
	TypeEntity* te = entity(1) ? entity(1)->asKind<TypeEntity>() : 0;
	if (!te)
		return;
	if (!_c && te->isType<Const>())
		te->asType<Const>()->unknit();
	else if (_c && !te->isType<Const>())
		te->insert(new Const);
}

void Memberify::setScope(Type const& _newScope)
{
	entity(1)->replace(TypeEntity::cloneOf(&*_newScope, owner()));
	QList<TypeEntity*> l;
	l << allEntitiesOf<TypeEntity>();
	while (!l.isEmpty())
	{
		TypeEntity* t = l.takeLast();
		l << t->allEntitiesOf<TypeEntity>();
		if (t->isKind<MemberTemplateType>())
			t->asKind<MemberTemplateType>()->substitute();
	}
}

Class* Memberify::scopeClass(bool* _isConst) const
{
	TypeEntity* te = entity(1) ? entity(1)->asKind<TypeEntity>() : 0;
	if (te && te->isType<ExplicitType>() && te->asType<ExplicitType>()->subject()->isKind<Class>())
	{
		if (_isConst)
			*_isConst = false;
		return te->asType<ExplicitType>()->subject()->asKind<Class>();
	}
	else if (te && te->isType<Const>() && te->ignore<Const>()->isType<ExplicitType>() && te->ignore<Const>()->asType<ExplicitType>()->subject()->isKind<Class>())
	{
		if (_isConst)
			*_isConst = true;
		return te->ignore<Const>()->asType<ExplicitType>()->subject()->asKind<Class>();
	}
	return 0;
}

void Memberify::setScopeClass(Class* _scope, bool _isConst)
{
	prepareChildren();
	if (entity(1))
		entity(1)->killAndDelete();
	middle(1).place(new ExplicitType(_scope));
	if (_isConst)
		entity(1)->insert(new Const);
}

Kinds Memberify::allowedKinds(int _i) const
{
	if (_i == 1)
		return Kind::of<TypeEntity>();
	return Super::allowedKinds(_i);
}

TypeEntity* Memberify::scopeType() const
{
	if (scope())
		if (scope()->isType<Const>())
			return scope()->asType<Const>()->child();
		else
			return scope();
	else
		return TypeEntity::null;
}

QString Memberify::code(QString const& _middle) const
{
	if (scopeType())
		return child()->code(" " + scopeType()->code() + ":: " + _middle) + (child()->ignore<ModifyingType>()->isType<FunctionType>() && isConst() ? " const" : "");
	return QString();
}

bool Memberify::defineSimilarityFrom(TypeEntity const* _f, Castability _c) const
{
	// const and mutable member methods fit in mutable-shaped holes.
	// only const member methods fit in const-shaped holes.
	// (Normal Logical castability test would be mutable->[const|mutable], const->[const]).
	// So... we do a reverse Logical castability test:
	// const->[const|mutable], mutable->[mutable]
	// Similarly, base-class memberified-methods fit into derived-shaped caller holes,
	// but not the other way around.
	return _f->isKind<Memberify>() && (!scope() || _f->asKind<Memberify>()->scope() && scope()->isSimilarTo(_f->asKind<Memberify>()->scope(), Physical))
				&& _f->asKind<Memberify>()->child()->isSimilarTo(child(), _c)
			|| Super::defineSimilarityFrom(_f, _c);
}

QString Memberify::modifierLayout() const
{
	if (!scopeClass())
		return QString();
	return "ycode;'" + scopeClass()->name() + "'";
}

TypeEntity* Memberify::newClone() const
{
	return new Memberify;
}

}
