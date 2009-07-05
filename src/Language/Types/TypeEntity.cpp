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

#include "CodeScene.h"
#include "SimpleType.h"
#include "ExplicitType.h"
#include "Pointer.h"
#include "Reference.h"
#include "Array.h"
#include "Const.h"
#include "Type.h"
#include "TypeEntity.h"

namespace Martta
{

MARTTA_OBJECT_CPP(TypeEntity);

TypeEntity* TypeEntity::null = (new TypeEntity)->setOwned();

bool TypeEntity::s_debugCastability = false;
int TypeEntity::s_typeCount = 0;

Types TypeEntity::assignableTypes() const
{
	return Types();
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

void TypeEntity::knitIn(ModifyingType* _t)
{
	if (owner())
	{
		_t->setOwner(owner());
		if (&**owner() == this)
			owner()->m_top = _t;
	}
	if (_t->child(ModifyingType::Original) && _t->child(ModifyingType::Original)->isPlaceholder())
		_t->child(ModifyingType::Original)->killAndDelete();
	insert(_t, ModifyingType::Original);
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
