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
#include "ExplicitType.h"
#include "CodeScene.h"
#include "CompletionDelegate.h"

namespace Martta
{

MARTTA_OBJECT_CPP(ExplicitType);

class ExplicitTypeSet: public IdentifierSet
{
public:
	virtual String						setId() const { return L"Martta::ExplicitType"; }
	virtual List<Named*>				identifiableAt(Position const& _p)
	{
		if (canPlaceVariable(_p))
			return list_cast<Named*>(castEntities<Identifiable>(_p.parent()->selfAndAncestorsChildrenOf<TypeDefinition>()));
		return List<Named*>();
	}
	virtual void						acceptAt(Position const& _pos, Named* _i)
	{
		placeVariable(_pos, new ExplicitType(static_cast<Identifiable*>(_i)->asKind<TypeDefinition>()));
	}
	virtual String						defineEditHtml(Named* _i, String const& _mid)
	{
		return ExplicitType(static_cast<Identifiable*>(_i)->asKind<TypeDefinition>()).fullHtml(_mid);
	}
	List<Named*> m_nameds;
};

static ExplicitTypeSet s_explicitTypeSet;

bool ExplicitType::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	if (_p.exists() && _p->isPlaceholder() && _e->text().length() == 1 && (_e->text()[0].isUpper() || _e->text()[0] == L':'))
	{
		// switch to Explicit Type.
		Entity* e = _p.place(new ExplicitType);
		_e->codeScene()->setEditing(e);
		if (_e->codeScene()->isEditing(e))
			_e->reinterpretLater();
	}
	else
		return false;
	return true;
}

bool ExplicitType::keyPressed(KeyEvent const* _e)
{
	if (_e->text().length() == 1 && (_e->text()[0].isLetter() || _e->text()[0] == L':'))
	{
		_e->codeScene()->setEditing(this);
		if (_e->codeScene()->isEditing(this))
			_e->reinterpretLater();
	}
	else
		return Super::keyPressed(_e);
	return true;
}

void ExplicitType::apresLoad()
{
	if (m_subject)
		set(m_subject);
	Super::apresLoad();
}

String ExplicitType::defineHtml() const
{
	return L"<^><span class=\"TypeEntity\">" + typeHtml(m_subject.isUsable() ? m_subject->name() : L"&empty;") + L"</span>";
}

List<TypeDefinition*> ExplicitType::possibilities() const
{
	List<TypeDefinition*> ret;
	TypeDefinition* old = m_subject;
	foreach (TypeDefinition* i, parent()->selfAndAncestorsChildrenOf<TypeDefinition>())
	{
		const_cast<ExplicitType*>(this)->m_subject = i;
		if (parent()->isChildInValidState(index()))
			ret << i;
	}
	const_cast<ExplicitType*>(this)->m_subject = old;
	return ret;
}

String ExplicitType::defineEditHtml(CodeScene* _cs) const
{
	if (EditDelegateFace* d = _cs->editDelegate(this))
		return tagOf(L"TypeEntity", typeHtml(d->real() + tagOf(L"unreal", d->unreal()))) + tagOf(L"minor", d->comment());
	return String::null;
}

EditDelegateFace* ExplicitType::newDelegate(CodeScene* _s)
{
	return new CompletionDelegate<ExplicitType, TypeDefinition*>(this, _s);
}

}
