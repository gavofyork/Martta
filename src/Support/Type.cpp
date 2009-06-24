/* ***** BEGIN LICENSE BLOCK *****
 * Version: Martta License version 1.0
 *
 * The contents of this file are subject to the Martta License version
 * 1.0 (the "License"); you may not use this file except in compliance with
 * the License. You should have received a copy of the Martta License
 * "COPYING.Martta" along with Martta; if not you may obtain a copy of the
 * License at http://quidprocode.co.uk/Martta/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Initial Developer of the code in this file is Gavin Wood.
 * Portions created by the Initial Developer are Copyright (C) 2008
 * quid pro code, Ltd. All Rights Reserved.
 *
 * ***** END LICENSE BLOCK ***** */

#include "CodeScene.h"
#include "TypeEntity.h"
#include "Reference.h"
#include "Pointer.h"
#include "Const.h"
#include "ExplicitType.h"
#include "SimpleType.h"
#include "Type.h"

namespace Martta
{

int g_typeCount = 0;

Type::Type(TypeDefinition* _subject):
	m_top(new ExplicitType(_subject))
{
	m_top->m_owner = this;
}

Type& Type::operator=(Type const& _t)
{
	M_ASSERT(m_top);
	if (m_top == _t.m_top)
		return *this;
	TypeEntity* buf = _t.m_top->clone(this);
	m_top->killAndDelete();
	m_top = buf;
	return *this;
}

bool Type::operator==(Type const& _t) const
{
	return m_top->isEquivalentTo(_t.m_top);
}

bool Type::isSimilarTo(Type const& _to, TypeEntity::Castability _c) const
{
	return m_top->isSimilarTo(_to.m_top, _c);
}

/**
 * Usage: full.strippedTo(empty)
 * empty must be ultimately null.
 * 
 * Will identify empty (ignoring Type()) with some portion in centre of full.
 * Everything between root and this portion will replace the Type() at root of empty.
 *
 * e.g.
 * <<int const*&>>.strippedTo(<<?*>>) == <<int const*>>
 * <<int*&>>.strippedTo(<<? const*>>) == <<?>>
 */
Type Type::strippedTo(Type const& _t) const
{
	M_ASSERT(_t.isUltimatelyNull());
	Type ret = *this;
	while (ret.m_top)
	{
		ModifyingType* j = _t.m_top->tryKind<ModifyingType>();
		for (ModifyingType* i = ret.m_top->tryKind<ModifyingType>(); i && j; i = i->original()->tryKind<ModifyingType>(), j = j->original()->tryKind<ModifyingType>())
			if (i->kind() != j->kind())
				goto BAD;
		if (!j)
			break;
		BAD:
		if (ret.m_top->isKind<ModifyingType>())
			ret.m_top->asKind<ModifyingType>()->unknit();
		else
			return Type();
	}
	return ret;
}

bool Type::isUltimatelyNull() const
{
	M_ASSERT(m_top);
	TypeEntity* t = m_top;
	while (t->isKind<ModifyingType>())
		t = t->asKind<ModifyingType>()->original();
	return t->isPlaceholder();
}

int Type::id(int _def) const
{
	if (m_top->isType<SimpleType>())
		return m_top->asType<SimpleType>()->id();
	if (m_top->isType<Pointer>())
		return Ptr;
	return _def;
}

}
