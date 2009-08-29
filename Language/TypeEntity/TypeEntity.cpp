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

#include "Type.h"
#include "TypeEntity.h"

namespace Martta
{

MARTTA_PLACEHOLDER_CPP(TypeEntity);
MARTTA_REGISTER_CSS(TypeEntity, ".TypeEntity { color: black; font-weight: bold; text-shadow: -1px -1px 0px #888; }");

TypeEntity* TypeEntity::null = (new TypeEntity)->setOwned();

bool TypeEntity::s_debugCastability = false;
int TypeEntity::s_typeCount = 0;

Types TypeEntity::assignableTypes() const
{
	return Types();
}

String TypeEntity::defineHtml() const
{
	return L"<^><span class=\"TypeEntity\">&empty;</span>";
}

bool TypeEntity::keyPressed(KeyEvent const* _e)
{
	if (attemptInsert(_e))
	{}
	else if (_e->isFocused() && !isPlaceholder() && _e->text().length() == 1 && _e->text()[0].isGraph())
	{
		_e->codeScene()->navigateAway(this, CodeScene::Forwards);
		if (_e->codeScene()->current() != this)
			_e->reinterpretLater();
	}
	else
		return Super::keyPressed(_e);
	return true;
}

void TypeEntity::parentAdded()
{
	if (m_isUnchanging != (parentIs<TypeEntity>() && parentAs<TypeEntity>()->m_isUnchanging))
		setInert(!m_isUnchanging);
	Super::parentAdded();
}

void TypeEntity::parentSwitched(Entity* _exParent)
{
	if (m_isUnchanging != (parentIs<TypeEntity>() && parentAs<TypeEntity>()->m_isUnchanging))
		setInert(!m_isUnchanging);
	Super::parentSwitched(_exParent);
}

String TypeEntity::typeHtml(String const& _middle) const
{
	return String(L"<span style=\"text-shadow: -1px -1px 0 %1\">").arg(idColour().name(255)) + _middle + L"</span>";
}

bool TypeEntity::isSimilarTo(TypeEntity const* _t, Castability _similarity) const
{
#if defined(DEBUG)
	static String s_ind = "        ";
	if (s_debugCastability)
	{
		mInfo() << (s_ind + "isSimilar from " + code() + " to " + _t->code() + " (for %1)").arg(_similarity);
		s_ind = "    " + s_ind;
	}
#endif
	bool c1 = _t->defineSimilarityFrom(this, _similarity);
	bool c2 = defineSimilarityTo(_t, _similarity);
#if defined(DEBUG)
	if (s_debugCastability)
	{
		s_ind = s_ind.mid(4);
		mInfo() << (s_ind + " = %1 (F -> T = %2, T <- F = %3)").arg(c2 || c1).arg(c2).arg(c1);
	}
#endif
	return c1 || c2;
}

TypeEntity* TypeEntity::newClone(Type* _o) const
{
	TypeEntity* ret;
	ret = newClone();
	ret->m_owner = _o;
	foreach (Entity* e, children())
		if (e->isKind<TypeEntity>())
			(e->index() < 0 ? ret->middle(e->index()) : ret->back()).place(e->asKind<TypeEntity>()->clone(_o));
		else
			break;
	return ret;
}

// Consider moving to ModifyingType & inverting?
void TypeEntity::knitIn(TypeEntity* _t)
{
	if (m_owner)
	{
		_t->setOwner(m_owner);
		if (&**m_owner == this)
			m_owner->m_top = _t;
	}
	if (_t->child(Default) && _t->child(Default)->isPlaceholder())
		_t->child(Default)->killAndDelete();
	insert(_t, Default);
}

bool TypeEntity::defineEquivalenceTo(TypeEntity const* _t) const
{
	if (_t->kind() != kind()) return false;
	if (!contentsEquivalentTo(_t)) return false;
	foreach (Entity* e, children())
		if (e->isKind<TypeEntity>() && _t->childIs<TypeEntity>(e->index()))
			if (!e->asKind<TypeEntity>()->isEquivalentTo(_t->childAs<TypeEntity>(e->index())))
				return false;
			else {}
		else
			if (e->isKind<TypeEntity>() != _t->childIs<TypeEntity>(e->index()))
				return false;
	return true;
}

}
