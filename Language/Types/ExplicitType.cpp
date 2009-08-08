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

#include "TypeDefinition.h"
#include "ExplicitType.h"
#include "CodeScene.h"
#include "CompletionDelegate.h"

namespace Martta
{

MARTTA_OBJECT_CPP(ExplicitType);

bool ExplicitType::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	if (_p.exists() && _p->isPlaceholder() && _e->text().length() == 1 && (_e->text()[0].isUpper() || _e->text()[0] == L':'))
	{
		// switch to Explicit Type.
		Entity* e = _p.place(new ExplicitType);
		e->setEditing(_e->codeScene());
		if (e->isEditing(_e->codeScene()))
			_e->reinterpretLater();
	}
	else
		return false;
	return true;
}

Types ExplicitType::assignableTypes() const
{
	return m_subject ? m_subject->assignableTypes() : Types();
}

List<ValueDefiner*> ExplicitType::applicableMembers(Entity const* _s, bool _isConst) const
{
	if (m_subject)
		return m_subject->applicableMembers(_s, _isConst);
	return List<ValueDefiner*>();
}

bool ExplicitType::hasSingleCastOperator(TypeEntity const* _t, bool _const) const
{
	if (!m_subject)
		return false;
	return m_subject->hasSingleCastOperator(_t, _const);
}

bool ExplicitType::hasDefaultConstructor() const
{
	if (!subject())
		return false;
	return subject()->hasDefaultConstructor();
}

bool ExplicitType::hasSingleConversionConstructor(TypeEntity const* _f) const
{
	if (!m_subject)
		return Unrelated;
	return subject()->hasSingleConversionConstructor(_f);
}

bool ExplicitType::defineSimilarityTo(TypeEntity const* _t, Castability _c) const
{
	if (m_subject)
		if ((_t->isKind<ExplicitType>() && m_subject == _t->asKind<ExplicitType>()->m_subject)
			|| m_subject->defineSimilarityTo(_t, _c))
			return true;

	return Super::defineSimilarityTo(_t, _c);
}

bool ExplicitType::defineSimilarityFrom(TypeEntity const* _f, Castability _c) const
{
	// TODO: only non-explicit
	return (m_subject && _c == Convertible && hasSingleConversionConstructor(_f)) ||
			Super::defineSimilarityFrom(_f, _c);
}

Rgb ExplicitType::idColour() const
{
	return 0xff7777;
}

bool ExplicitType::canStandAlone() const
{
	return m_subject && m_subject->canStandAlone();
}

bool ExplicitType::keyPressed(KeyEvent const* _e)
{
	if (_e->text().length() == 1 && (_e->text()[0].isUpper() || _e->text()[0] == L':'))
	{
		setEditing(_e->codeScene());
		if (isEditing(_e->codeScene()))
			_e->reinterpretLater();
	}
	else
		return Super::keyPressed(_e);
	return true;
}

List<Declaration*> ExplicitType::utilised() const
{
	return m_subject.isUsable() ? subject()->utilisedInUse() : Super::utilised();	// TODO: define for other types.
}

String ExplicitType::defineLayout(ViewKeys const&) const
{
	return "^;fb;s" + idColour().name() + ";'" + (m_subject.isUsable() ? m_subject->name() : "[]") + "'";
}

String ExplicitType::defineHtml() const
{
	return L"<span id=\"this\" class=\"TypeEntity\">" + typeHtml(m_subject.isUsable() ? m_subject->name() : L"&empty;") + L"</span>";
}

List<TypeDefinition*> ExplicitType::possibilities()
{
	List<TypeDefinition*> ret;
	TypeDefinition* old = m_subject;
	foreach (TypeDefinition* i, parent()->selfAndAncestorsChildrenOf<TypeDefinition>())
	{
		m_subject = i;
		if (parent()->isChildInValidState(index()))
			ret << i;
	}
	m_subject = old;
	return ret;
}

String ExplicitType::defineEditLayout(ViewKeys const&, TypeDefinition*) const
{
	return "fb;s" + idColour().name() + ";^;%1";
}

String ExplicitType::defineEditHtml(TypeDefinition*) const
{
	return L"<span class=\"TypeEntity\">" + typeHtml(L"<?>") + L"</span>";
}

EditDelegateFace* ExplicitType::newDelegate(CodeScene* _s)
{
	return new CompletionDelegate<ExplicitType, TypeDefinition*>(this, _s);
}

}
