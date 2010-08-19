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

#include "Labelled.h"
#include "CodeScene.h"
#include "EditDelegate.h"
#include "CompletionDelegate.h"
#include "ReferencedValue.h"

namespace Martta
{

MARTTA_PLACEHOLDER_CPP(ReferencedValue);
MARTTA_REGISTER_CSS(ReferencedValue, ".ReferencedValue { font-weight: normal; color: #000; }");

ReferencedValue::ReferencedValue(ValueDefiner* _v):
	m_subject	(0)
{
	set(_v);
}

bool ReferencedValue::isSuperfluous() const
{
	return (!m_subject.isUsable() && m_hopeful.isEmpty()) || Super::isSuperfluous();
}

String ReferencedValue::code() const
{
	if (m_subject.isUsable())
		return m_subject->reference();
	else
		return String::null;
}

void ReferencedValue::onDependencySwitched(Concept* _t, Concept* _old)
{
//	mDebug() << "Dependency switched" << _old << "->" << _t << " subject:" << (m_subject ? m_subject->self() : (Concept*)0);
	if (m_subject == _old->tryKind<ValueDefiner>())
	{
//		mDebug() << "Subject switched to" << _t << "from" << _old;
		set(_t->tryKind<ValueDefiner>());
	}
}

void ReferencedValue::onDependencyRemoved(Concept* _old, int)
{
//	mDebug() << "Dependency removed" << _old << " subject:" << &*m_subject;
	if (m_subject == _old->tryKind<ValueDefiner>())
	{
//		mDebug() << "Subject removed (was" << _old << ")";
		if (Identifiable* i = _old->tryKind<Identifiable>())
			m_hopeful = i->nick();
		set(0);
	}
}

bool ReferencedValue::isInValidState() const
{
	return m_subject && m_subject->isAccessibleAt(over()) && Super::isInValidState();
}

Type ReferencedValue::bareType() const
{
	// If we're not referencing anything yet, return null.
	if (!m_subject.isUsable())
		return Type();
	return m_subject->bareType();
}

Type ReferencedValue::type() const
{
	// If we're not referencing anything yet, return null.
	if (!m_subject.isUsable())
		return Type();
	return m_subject->type();
}

String ReferencedValue::defineHtml() const
{
	if (!m_subject)
		return L"<^><span class=\"unreal\">[" + m_hopeful + L"?]</span>";
	return L"<^><span class=\"ReferencedValue\">" +
		m_subject->type()->typeHtml(m_subject->tryKind<Labelled>()->labelHtml(m_subject->nick())) +
		L"</span>";
}

EditDelegateFace* ReferencedValue::newDelegate(CodeScene* _s)
{
	return new CompletionDelegate<ReferencedValue, ValueDefiner*>(this, _s);
}

String ReferencedValue::editHtmlHelper(ValueDefiner* _v, String const& _mid) const
{
	String ret = (_v ? &*_v->type() : TypeConcept::null)->typeHtml(_mid);
	return tagOf(L"ReferencedValue", _v->tryKind<Labelled>()->labelHtml(ret));
}

String ReferencedValue::defineEditHtml(CodeScene* _cs) const
{
	if (EditDelegateFace* d = _cs->editDelegate(this))
		return editHtmlHelper(static_cast<CompletionDelegate<ReferencedValue, ValueDefiner*>*>(d)->selection(), d->real() + tagOf(L"_ms_unreal", d->unreal())) + tagOf(L"_ms_minor", d->comment());
	return String::null;
}

}
