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
#include "Referenced.h"

namespace Martta
{

MARTTA_PLACEHOLDER_CPP(Referenced);
MARTTA_REGISTER_CSS(Referenced, ".Referenced { font-weight: normal; color: #000; }");

Referenced::Referenced(ValueDefiner* _v):
	m_subject	(0)
{
	setSubject(_v);
}

bool Referenced::isSuperfluous() const
{
	return (!m_subject.isUsable() && m_hopeful.isEmpty()) || Super::isSuperfluous();
}

String Referenced::code() const
{
	if (m_subject.isUsable())
		return m_subject->reference();
	else
		return String::null;
}

void Referenced::onDependencySwitched(Entity* _t, Entity* _old)
{
//	mDebug() << "Dependency switched" << _old << "->" << _t << " subject:" << (m_subject ? m_subject->self() : (Entity*)0);
	if (m_subject->self() == _old)
	{
//		mDebug() << "Subject switched to" << _t << "from" << _old;
		setSubject(_t->asKind<ValueDefiner>());
	}
}

void Referenced::onDependencyRemoved(Entity* _old, int)
{
//	mDebug() << "Dependency removed" << _old << " subject:" << &*m_subject;
	if (m_subject->self() == _old)
	{
//		mDebug() << "Subject removed (was" << _old << ")";
		if (Identifiable* i = _old->tryKind<Identifiable>())
			m_hopeful = i->name();
		setSubject(0);
	}
}

bool Referenced::isInValidState() const
{
	// If we're not referencing anything yet, return null.
	if (!m_subject.isUsable())
		return false;
	return Super::isInValidState();
}

Type Referenced::type() const
{
	// If we're not referencing anything yet, return null.
	if (!m_subject.isUsable())
		return Type();
	return m_subject->type();
}

String Referenced::defineHtml() const
{
	if (!m_subject)
		return L"<span id=\"this\" class=\"unreal\">[" + m_hopeful + L"?]</span>";
	return L"<span id=\"this\" class=\"Referenced\">" + m_subject->tryKind<Labelled>()->labelHtml(m_subject->type()->typeHtml(m_subject->name())) + L"</span>";
}

EditDelegateFace* Referenced::newDelegate(CodeScene* _s)
{
	return new CompletionDelegate<Referenced, ValueDefiner*>(this, _s);
}

String Referenced::editHtmlHelper(ValueDefiner* _v, String const& _mid) const
{
	String ret = (_v ? &*_v->type() : TypeEntity::null)->typeHtml(_mid);
	if (_v)
		return L"<span class=\"Referenced\">" + _v->tryKind<Labelled>()->labelHtml(ret) + "</span>";
	return ret;
}

String Referenced::defineEditHtml(CodeScene* _cs) const
{
	if (EditDelegateFace* d = editDelegate(_cs))
		return editHtmlHelper(static_cast<CompletionDelegate<Referenced, ValueDefiner*>*>(d)->selection(), d->real() + L"<span class=\"unreal\">" + d->unreal() + L"</span>");
	return String::null;
}

}
