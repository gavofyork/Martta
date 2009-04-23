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
	return isValid() && m_index >= 0 && m_context->entity(m_index);
}

Entity* InsertionPoint::entity() const
{
	M_ASSERT(exists());
	return m_context->entity(m_index);
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
	if (m_context)
		m_context->childAdded(ret);
	ret->contextAdded();
	return ret;
}

Entity* InsertionPoint::place(Entity* _e) const
{
	M_ASSERT(_e);
	Entity* oc = _e->context();
	int oci = _e->contextIndex();
	
	if (exists() && entity()->isPlaceholder())
		entity()->replace(_e);
	else
	{
		_e->setContext(m_context);
		if (m_context && m_index > -1)
			_e->moveToPosition(m_index);
		if (oc != m_context)
			_e->contextSwitchedWithChildRemoved(oc, oci);
		m_context->childAdded(m_index);
	}
	return _e;
}

void InsertionPoint::insertSilent(Entity* _e) const
{
	M_ASSERT(_e);
	_e->setContext(m_context);
	if (m_context && m_index > -1)
		_e->moveToPosition(m_index);
}

bool InsertionPoint::allowedToBeKind(Kind _k) const
{
	return _k.isKind(m_context->allowedKinds(m_index < 0 ? m_context->entities().size() : m_index))
			&& !_k.isKind(m_context->deniedKinds(m_index < 0 ? m_context->entities().size() : m_index));
}

bool InsertionPoint::isRequired() const
{
	return (m_index < 0 ? m_context->entities().size() : m_index) < m_context->minimumRequired();
}

Kinds InsertionPoint::allowedKinds() const
{
	return m_context->allowedKinds(m_index < 0 ? m_context->entities().size() : m_index);
}

Kinds InsertionPoint::deniedKinds() const
{
	return m_context->deniedKinds(m_index < 0 ? m_context->entities().size() : m_index);
}

Entity* InsertionPoint::nearestEntity() const
{
	if (m_index < m_context->entities().size())
		return m_context->entity(m_index);
	else if (m_context->entities().size())
		return m_context->entities().last();
	else
		return m_context;
}

QDebug operator<<(QDebug _out, InsertionPoint const& _item)
{
	_out << _item.m_index << "@" << &*_item.m_context;
	if (_item.exists())
		_out << "[" << _item.entity() << "]";
	return _out;
}

}
