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

#include "Artificial.h"
#include "Class.h"
#include "GenericMemberOperation.h"
#include "Field.h"
#include "MemberLambda.h"

#include "BuiltinType.h"

#include "Memberify.h"
#include "FunctionType.h"
#include "Reference.h"
#include "Typed.h"
#include "CodeScene.h"

#include "CompletionDelegate.h"
#include "MemberReferencedValue.h"

namespace Martta
{

MARTTA_PROPER_CPP(MemberReferencedValue);

static ReferencedValueSet<FloatingMemberReferencedValue> s_memberReferencedValueRegistrand;

MemberReferencedValue::MemberReferencedValue(ValueDefiner* _subject):
	ReferencedValue(_subject)
{
}

bool MemberReferencedValue::keyPressed(KeyEvent const* _e)
{
	if (_e->text() == L"`")
	{
		_e->codeScene()->setCurrent(replace(new FloatingMemberReferencedValue(m_subject)));
		return true;
	}
	return Super::keyPressed(_e);
}

Kinds MemberReferencedValue::ancestralDependencies() const
{
	Kinds ret = Kind::of<MemberLambda>();
	if (!m_subject.isNull() && !parentIs<GenericMemberOperation>() && m_subject->isKind<MemberValue>() && hasAncestor<MemberLambda>() && ancestor<TypeDefinition>() != m_subject->asKind<MemberValue>()->typeDefinition())
		ret << Kind::of<Class>();
	return ret;
}

List<ValueDefiner*> MemberReferencedValue::staticRefPossibilities(Position const& _p, bool _methods, bool _nonMethods)
{
	List<ValueDefiner*> ret;
	Type method = Type(WildInvocableType()).topWith(Memberify()).topWith(Reference());
	if (TypedOwner* s = _p.parent()->tryKind<TypedOwner>())
		foreach (Type t, s->allowedTypes(_p.index()))
		{
			List<ValueDefiner*> appMems;
			if (t->isType<Memberify>() && t->asType<Memberify>()->scope() && _p.concept())
				appMems = t->asType<Memberify>()->scope()->applicableMembers(_p.concept(), t->asType<Memberify>()->isConst());
			else if (Class* c = _p->ancestor<Class>())
				appMems = concepts_cast<ValueDefiner>(c->membersOf<MemberValue>(_p->hasAncestor<MemberLambda>() ? _p->ancestor<MemberLambda>()->isConst() : false));
			appMems = filterConceptsInv<Artificial>(appMems);
			if (!_methods)
				appMems = filterTypedsInv<ValueDefiner>(method, appMems);
			else if (!_nonMethods)
				appMems = filterTypeds<ValueDefiner>(method, appMems);
			mDebug() << appMems.count();
			ret << appMems;
		}
	return ret;
}

Type MemberReferencedValue::apparentType() const
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

String MemberReferencedValue::defineEditHtml(CodeScene* _cs) const
{
//	if (EditDelegateFace* d = _cs->editDelegate(this))
//		if (!static_cast<CompletionDelegate<ReferencedValue, ValueDefiner*>*>(d)->selection())
//			return Field().labelHtml(d->real() + tagOf(L"unreal", d->unreal()));
	return Super::defineEditHtml(_cs);
}

/*	else if (_e->text() == " " || _e->text() == "[" || _e->text() == "#")
	{
		codeScene()->leaveEdit();
		_e->reinterpretLater();	// since we may have been deleted and replaced with e.g. a variable, we want appropriately.
		return true;
	}*/

MARTTA_PROPER_CPP(FloatingMemberReferencedValue);

bool FloatingMemberReferencedValue::keyPressed(KeyEvent const* _e)
{
	if (_e->text() == L"`")
	{
		_e->codeScene()->setCurrent(replace(new MemberReferencedValue(m_subject)));
		return true;
	}
	return Super::keyPressed(_e);
}

String FloatingMemberReferencedValue::code() const
{
	if (!m_subject.isNull())
		return m_subject->nonSpecificReference();
	else
		return String::null;
}

bool FloatingMemberReferencedValue::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	if (_p.exists() && _p->isPlaceholder() && _e->text().length() == 1 && _e->text()[0].isLower() &&
		_p->isKind<Typed>() && _p->asKind<Typed>()->ourAllowedTypes().size() && _p->asKind<Typed>()->ourAllowedTypes()[0]->isType<Memberify>())
	{
		FloatingMemberReferencedValue* r = new FloatingMemberReferencedValue;
		_p.place(r);
		_e->codeScene()->setEditing(r);
		if (_e->codeScene()->isEditing(r))
			_e->reinterpretLater();
	}
	else if (_p.exists() && _p->isPlaceholder() && _e->text() == L"M")
	{
		FloatingMemberReferencedValue* r = new FloatingMemberReferencedValue;
		_p.place(r);
		_e->codeScene()->setEditing(r);
	}
	else
		return false;
	return true;
}

}
