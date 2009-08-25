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
#include "MemberVariable.h"
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

static ReferencedValueSet<MemberReferenced> s_memberReferencedRegistrand;

bool MemberReferenced::keyPressed(KeyEvent const* _e)
{
	if (_e->text() == L"`")
	{
		replace(new FloatingMemberReferenced(m_subject))->setCurrent();
		return true;
	}
	return Super::keyPressed(_e);
}

Kinds MemberReferenced::ancestralDependencies() const
{
	Kinds ret = Kind::of<MemberLambda>();
	if (!m_subject.isNull() && !parentIs<GenericMemberOperation>() && m_subject->isKind<MemberValue>() && hasAncestor<MemberLambda>() && ancestor<TypeDefinition>() != m_subject->asKind<MemberValue>()->typeDefinition())
		ret << Kind::of<Class>();
	return ret;
}

List<ValueDefiner*> MemberReferenced::possibilities(Position const& _p, bool _methods, bool _nonMethods)
{
	List<ValueDefiner*> ret;
	Type method = Type(FunctionType(false, true)).topWith(Memberify()).topWith(Reference());
	mInfo() << _p.entity() << _p->tryKind<TypeNamer>();
	if (TypedOwner* s = _p.parent()->tryKind<TypedOwner>())
		foreach (Type t, s->allowedTypes(_p.index()))
		{
			List<ValueDefiner*> appMems;
			if (t->isType<Memberify>() && t->asType<Memberify>()->scope() && _p.entity())
				appMems = t->asType<Memberify>()->scope()->applicableMembers(_p.entity(), t->asType<Memberify>()->isConst());
			else if (Class* c = _p->ancestor<Class>())
				appMems = castEntities<ValueDefiner>(c->membersOf<MemberValue>(_p->hasAncestor<MemberLambda>() ? _p->ancestor<MemberLambda>()->isConst() : false));
			mInfo() << appMems << _p->ancestor<Class>();
			if (!_methods)
				appMems = filterTypedsInv<ValueDefiner>(method, appMems);
			else if (!_nonMethods)
				appMems = filterTypeds<ValueDefiner>(method, appMems);
			ret << appMems;
		}
	return ret;
}

Type MemberReferenced::apparentType() const
{
	Type ret = type();
	// If we're not in a member operation, check if there's some memberification that we can silently discard;
	if (!parentIs<GenericMemberOperation>() && ret->isType<Memberify>() && hasAncestor<MemberLambda>())
	{
		// switch in params, put in registrable iface....
		AssertNR(hasAncestor<Class>());
		// There is; check to see if we can remove it (by being in a scoped parent and assuming the "this->" precedent).
		Memberify* m = ret->asType<Memberify>();
		if (ancestor<Class>()->baseAccess(m->scope<Class>()) <= Protected)
		{
			bool memberIsCallable = m->original()->isType<FunctionType>();
			bool constScope = ancestor<MemberLambda>()->isConst();
			bool constMember = memberIsCallable ? m->isConst() : m->original()->isType<Const>();
			if (constMember || (!constMember && !constScope))
			{
				// Member Variable/FunctionType inside a method. Either enclosing method is non-const or FunctionType is const.
				// Allowed.
				m->unknit();
			}
			else if (!memberIsCallable && constScope && !constMember)
			{
				// Member Variable referenced inside a const method
				// Allowed but made const.
				m->original()->knit<Const>();
				m->unknit();
			}
		}
	}
	return ret;
}

String MemberReferenced::defineEditHtml(CodeScene* _cs) const
{
	if (EditDelegateFace* d = _cs->editDelegate(this))
	{
		ValueDefiner* s = static_cast<CompletionDelegate<Referenced, ValueDefiner*>*>(d)->selection();
		String ret = d->real() + L"<span class=\"unreal\">" + d->unreal() + L"</span>";
		if (!s)
			ret = MemberVariable().labelHtml(ret);
		return editHtmlHelper(s, ret);
	}
	return String::null;
}

/*	else if (_e->text() == " " || _e->text() == "[" || _e->text() == "#")
	{
		codeScene()->leaveEdit();
		_e->reinterpretLater();	// since we may have been deleted and replaced with e.g. a variable, we want appropriately.
		return true;
	}*/

MARTTA_OBJECT_CPP(FloatingMemberReferenced);

bool FloatingMemberReferenced::keyPressed(KeyEvent const* _e)
{
	if (_e->text() == L"`")
	{
		replace(new MemberReferenced(m_subject))->setCurrent();
		return true;
	}
	return Super::keyPressed(_e);
}

String FloatingMemberReferenced::code() const
{
	if (!m_subject.isNull())
		return m_subject->nonSpecificReference();
	else
		return String::null;
}

bool FloatingMemberReferenced::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	if (_p.exists() && _p->isPlaceholder() && _e->text().length() == 1 && _e->text()[0].isLower() &&
		_p->isKind<Typed>() && _p->asKind<Typed>()->ourAllowedTypes().size() && _p->asKind<Typed>()->ourAllowedTypes()[0]->isType<Memberify>())
	{
		FloatingMemberReferenced* r = new FloatingMemberReferenced;
		_p.place(r);
		_e->codeScene()->setEditing(r);
		if (_e->codeScene()->isEditing(r))
			_e->reinterpretLater();
	}
	else if (_p.exists() && _p->isPlaceholder() && _e->text() == L"M")
	{
		FloatingMemberReferenced* r = new FloatingMemberReferenced;
		_p.place(r);
		_e->codeScene()->setEditing(r);
	}
	else
		return false;
	return true;
}

}
