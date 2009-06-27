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

#include "InsertionPoint.h"

#include "Entity.h"

namespace Martta
{

bool InsertionPoint::exists() const
{
	return isValid() && m_index != UndefinedIndex && m_parent->child(m_index);
}

Entity* InsertionPoint::entity() const
{
	M_ASSERT(exists());
	return m_parent->child(m_index);
}

Entity* InsertionPoint::spawnPreparedSilent() const
{
	Entity* ret = Entity::spawn(allowedKinds().commonBase().name());
	ret->prepareChildren();
	insertSilent(ret);
	return ret;
}

Entity* InsertionPoint::spawnPrepared() const
{
	Entity* ret = spawnPreparedSilent();
	if (m_parent)
		m_parent->childAdded(ret->index());
	ret->parentAdded();
	return ret;
}

Entity* InsertionPoint::place(Entity* _e) const
{
	M_ASSERT(_e);
	_e->move(*this);
	return _e;
}

void InsertionPoint::insertSilent(Entity* _e) const
{
	M_ASSERT(_e);
	_e->silentMove(*this);
}

bool InsertionPoint::allowedToBeKind(Kind _k) const
{
	return _k.isKind(m_parent->allowedKinds(m_index == UndefinedIndex ? m_parent->cardinalChildCount() : m_index))
			&& !_k.isKind(m_parent->deniedKinds(m_index == UndefinedIndex ? m_parent->cardinalChildCount() : m_index));
}

bool InsertionPoint::isRequired() const
{
	return m_index < 0 ? (m_parent->childCountAt(m_index) < m_parent->minRequired(m_index)) : ((m_index == UndefinedIndex ? m_parent->cardinalChildCount() : m_index) < m_parent->minRequired());
}

Kinds InsertionPoint::allowedKinds() const
{
	return m_parent->allowedKinds(m_index == UndefinedIndex ? m_parent->cardinalChildCount() : m_index);
}

Kinds InsertionPoint::deniedKinds() const
{
	return m_parent->deniedKinds(m_index == UndefinedIndex ? m_parent->cardinalChildCount() : m_index);
}

Entity* InsertionPoint::nearestEntity() const
{
	if (m_parent->child(m_index))
		return m_parent->child(m_index);
	else if (m_parent->cardinalChildCount())
		return m_parent->cardinalChildren().last();
	else
		return m_parent;
}

QDebug operator<<(QDebug _out, InsertionPoint const& _item)
{
	_out << _item.m_index << "@" << &*_item.m_parent;
	if (_item.exists())
		_out << "[" << _item.entity() << "]";
	return _out;
}

}
