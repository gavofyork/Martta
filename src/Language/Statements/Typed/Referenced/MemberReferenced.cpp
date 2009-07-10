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
#include "GenericMemberOperation.h"
#include "MemberLambda.h"

#include "Memberify.h"
#include "FunctionType.h"
#include "Reference.h"
#include "Typed.h"
#include "CodeScene.h"

#include "CompletionDelegate.h"
#include "MemberReferenced.h"

namespace Martta
{

MARTTA_OBJECT_CPP(MemberReferenced);

bool MemberReferenced::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	if (_p.exists() && _p->isPlaceholder() && _e->text().length() == 1 && _e->text()[0].isLower() &&
		_p->isKind<Typed>() && _p->asKind<Typed>()->ourAllowedTypes().size() && _p->asKind<Typed>()->ourAllowedTypes()[0]->isType<Memberify>())
	{
		_e->reinterpretLater();
		MemberReferenced* r = new MemberReferenced;
		_p.place(r);
		r->setEditing(_e->codeScene());
	}
	else if (_p.exists() && _p->isPlaceholder() && _e->text() == L"M")
	{
		MemberReferenced* r = new MemberReferenced;
		_p.place(r);
		r->setEditing(_e->codeScene());
	}
	else
		return false;
	return true;
}

Kinds MemberReferenced::ancestralDependencies() const
{
	Kinds ret = Kind::of<MemberLambda>();
	if (!m_subject.isNull() && !parentIs<GenericMemberOperation>() && m_subject->isKind<MemberValue>() && hasAncestor<MemberLambda>() && ancestor<Class>() != m_subject->asKind<MemberValue>()->classType())
		ret << Kind::of<Class>();
	return ret;
}

bool MemberReferenced::isInValidState() const
{
	if (!parentIs<GenericMemberOperation>() && m_subject->isKind<MemberValue>() && hasAncestor<MemberLambda>())
	{
		AssertNR(hasAncestor<Class>());
		if (!ancestor<Class>()->membersOf<MemberValue>(ancestor<MemberLambda>()->isConst()).contains(m_subject->asKind<MemberValue>()))
			return false;
	}
	return Super::isInValidState();
}

String MemberReferenced::code() const
{
	if (!m_subject.isNull())
		return m_subject->nonSpecificReference();
	else
		return String::null;
}

/*
// x => this->x mutator
void ClassMutator::listPossibleMutations(Position const& _p, Type const& _original, Types& _mutations)
{
	// If we're not in a member operation, check if there's some memberification that we can silently discard; 
	if (!_p->parentIs<GenericMemberOperation>() && _original->isType<Memberify>() && _p->hasAncestor<MemberLambda>())
	{
		// switch in params, put in registrable iface....
		AssertNR(_p->hasAncestor<Class>());
		// There is; check to see if we can remove it (by being in a scoped parent and assuming the "this->" precedent).
		Type ret = _original;
		Memberify* m = ret->asType<Memberify>();
		if (_p->ancestor<Class>()->baseAccess(m->scope<Class>()) <= Protected)
		{
			bool memberIsCallable = m->original()->isType<FunctionType>();
			bool constScope = _p->ancestor<MemberLambda>()->isConst();
			bool constMember = memberIsCallable ? m->isConst() : m->original()->isType<Const>();
			if (constMember || !constMember && !constScope)
			{
				// Member Variable/FunctionType inside a method. Either enclosing method is non-const or FunctionType is const.
				// Allowed.
				m->unknit();
				_mutations << ret;
			}
			else if (!memberIsCallable && constScope && !constMember)
			{
				// Member Variable referenced inside a const method
				// Allowed but made const.
				m->original()->knit<Const>();
				m->unknit();
				_mutations << ret;
			}
		}
	}
}
*/

EditDelegateFace* MemberReferenced::newDelegate(CodeScene* _s)
{
	return new CompletionDelegate<MemberReferenced, ValueDefiner*>(this, _s);
}

List<ValueDefiner*> MemberReferenced::possibilities() const
{
	List<ValueDefiner*> ret;
	Type method = Type(FunctionType(false, true)).topWith(Memberify()).topWith(Reference());
	foreach (Type t, ourAllowedTypes())
	{
		List<ValueDefiner*> appMems;
		if (t->isType<Memberify>() && t->asType<Memberify>()->scope())
			appMems = t->asType<Memberify>()->scope()->applicableMembers(this, t->asType<Memberify>()->isConst());
		else if (hasAncestor<Class>())
			appMems = castEntities<ValueDefiner>(ancestor<Class>()->membersOf<MemberValue>(hasAncestor<MemberLambda>() ? ancestor<MemberLambda>()->isConst() : false));
		if (false)
			appMems = filterTypedsInv<ValueDefiner>(method, appMems);
		else if (false)
			appMems = filterTypeds<ValueDefiner>(method, appMems);
		ret << appMems;
	}
	return ret;
}

String MemberReferenced::defineEditLayout(ViewKeys const& _k, ValueDefiner* _v)
{
	String ret = L"^;s" + (_v ? _v->type()->idColour() : TypeEntity::null->idColour()).name() + L";c;%1";
	if (_v)
		ret = _v->tryKind<Labelled>()->labelLayout(ret, _k);
	return ret;
}

void MemberReferenced::committed()
{
/*	else if (_e->text() == " " || _e->text() == "[" || _e->text() == "#")
	{
		codeScene()->leaveEdit();
		_e->reinterpretLater();	// since we may have been deleted and replaced with e.g. a variable, we want appropriately.
		return true;
	}*/
}

}
