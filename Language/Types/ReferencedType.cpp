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

#include "VariablePlacer.h"

#include "IdentifierSet.h"
#include "TypeDefinition.h"
#include "ReferencedType.h"
#include "CodeScene.h"
#include "CompletionDelegate.h"

namespace Martta
{

MARTTA_PROPER_CPP(ReferencedType);

class ReferencedTypeSet: public IdentifierSet
{
public:
	virtual String						setId() const { return L"Martta::ReferencedType"; }
	virtual List<Named*>				identifiableAt(Position const& _p)
	{
		if (canPlaceVariable(_p))
			return list_cast<Named*>(concepts_cast<Identifiable>(_p.parent()->selfAndAncestorsChildrenOf<TypeDefinition>()));
		return List<Named*>();
	}
	virtual void						acceptAt(Position const& _pos, Named* _i, CodeScene* _cs)
	{
		placeVariable(_pos, new ReferencedType(static_cast<Identifiable*>(_i)->asKind<TypeDefinition>()), _cs);
	}
	virtual String						defineEditHtml(Named* _i, String const& _mid)
	{
		return ReferencedType(static_cast<Identifiable*>(_i)->asKind<TypeDefinition>()).fullHtml(WebViewable::composeName(_mid, static_cast<Identifiable*>(_i)->kind().realSupers().names().reversed()));
	}
	List<Named*> m_nameds;
};

static ReferencedTypeSet s_explicitTypeSet;

bool ReferencedType::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	if (_p.exists() && _p->isPlaceholder() && _e->text().length() == 1 && (_e->text()[0].isUpper() || _e->text()[0] == L':'))
	{
		// switch to Explicit Type.
		Concept* e = _p.place(new ReferencedType);
		_e->codeScene()->setEditing(e);
		if (_e->codeScene()->isEditing(e))
			_e->reinterpretLater();
	}
	else
		return false;
	return true;
}

bool ReferencedType::keyPressed(KeyEvent const* _e)
{
	if (_e->text().length() == 1 && (_e->text()[0].isLetter() || _e->text()[0] == L':'))
	{
		_e->codeScene()->setEditing(this);
		if (_e->codeScene()->isEditing(this))
			_e->reinterpretLater();
	}
	else if (_e->text() == L"@" && m_subject)
		_e->codeScene()->navigateOnto(m_subject);
	else
		return Super::keyPressed(_e);
	return true;
}

void ReferencedType::apresLoad()
{
	if (m_subject)
		set(m_subject);
	Super::apresLoad();
}

String ReferencedType::defineHtml() const
{
	//return L"<^>" + tagOf(L"TypeConcept", typeHtml(m_subject.isUsable() ? m_subject->tryKind<Labelled>()->labelHtml(m_subject->nick()) : L"&empty;"));
	return L"<^>" + tagOf(L"TypeConcept", m_subject.isUsable() ? m_subject->tryKind<Labelled>()->labelHtml(m_subject->nick()) : typeHtml(L"&empty;"));
}

List<TypeDefinition*> ReferencedType::possibilities() const
{
	List<TypeDefinition*> ret;
	TypeDefinition* old = m_subject;
	foreach (TypeDefinition* i, parent()->selfAndAncestorsChildrenOf<TypeDefinition>())
	{
		const_cast<ReferencedType*>(this)->m_subject = i;
		if (parent()->isChildInValidState(index()))
			ret << i;
	}
	const_cast<ReferencedType*>(this)->m_subject = old;
	return ret;
}

String ReferencedType::defineEditHtml(CodeScene* _cs) const
{
	if (EditDelegateFace* d = _cs->editDelegate(this))
		return tagOf(L"TypeConcept", typeHtml(d->real() + tagOf(L"_ms_unreal", d->unreal()))) + tagOf(L"_ms_minor", d->comment());
	return String::null;
}

EditDelegateFace* ReferencedType::newDelegate(CodeScene* _s)
{
	return new CompletionDelegate<ReferencedType, TypeDefinition*>(this, _s);
}

}
