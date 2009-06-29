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

#pragma once

#include <climits>

#include <QList>

#include "SafePointer.h"
#include "Kind.h"

namespace Martta
{

static const int UndefinedIndex = INT_MAX;

class Entity;
	
class InsertionPoint
{
	friend class Entity;
	friend QDebug operator<<(QDebug _out, InsertionPoint const& _item);
	
public:
	InsertionPoint(): m_parent(0), m_index(UndefinedIndex) {}
	InsertionPoint(InsertionPoint const& _c): m_parent(_c.m_parent), m_index(_c.m_index) {}
	InsertionPoint& operator=(InsertionPoint const& _c) { m_parent = _c.m_parent; m_index = _c.m_index; return *this; }
	
	inline bool							operator==(InsertionPoint const& _c) const { return _c.m_parent == m_parent && _c.m_index == m_index; }
	inline bool							operator!=(InsertionPoint const& _c) const { return !operator==(_c); }
	
	inline 								operator bool() const { return isValid(); }
	template<class T> inline bool		allowedToBeKind() const { return allowedToBeKind(Kind::of<T>()); }
	bool								allowedToBeKind(Kind _k) const;
	Kinds								allowedKinds() const;
	Kinds								deniedKinds() const;
	bool								isRequired() const;
	Entity*								nearestEntity() const;

	Entity&								operator*() const { return *entity(); }
	Entity*								operator->() const { return entity(); }
	bool								exists() const;
	Entity*								entity() const;

	inline bool							isNull() const { return !m_parent; }
	inline bool							isValid() const { return m_parent; }
	
	/**
	 * Inserts @a _e into the parent's entity list, so it becomes at the position
	 * required.
	 *
	 * @returns @a _e.
	 *
	 * If a placeholder exists already at that position it is deleted first.
	 */
	Entity*								place(Entity* _e) const;
	
	/**
	 * Inserts @a _e into the parent's entity list, so it becomes at the position
	 * required.
	 *
	 * Ignores the placeholders (if any) that might exist already at that position.
	 */
	Entity*								insert(Entity* _e) const;
	
	/**
	 * Inserts @a _e into the parent's entity list, so it becomes at the position
	 * required.
	 *
	 * Ignores the placeholders (if any) that might exist already at that position.
	 * 
	 * Unlike place() and insert(), this doesn't issue any notifications. Make sure
	 * you call any required notifications afterwards on all affected entities.
	 *
	 * You'll probably want to use insert/place rather than this generally.
	 */
	void								insertSilent(Entity* _e) const;

	inline Entity*						parent() const { return m_parent; }
	inline int							index() const { return m_index; }

	/**
	 * This will not call changed() as appropriate.
	 */
	Entity*								spawnPreparedSilent() const;

	/**
	 * This will call changed() as appropriate.
	 */
	Entity*								spawnPrepared() const;

	
	// TODO: Bring these in once I'm comfortable with them.
//	template<class T> inline T*			operator<<=(T* _e) const { place(_e); return _e; }
//	template<class T> inline InsertionPoint const& operator<<(T* _e) const { place(_e); return InsertionPoint(m_parent, (m_index == -1) ? -1 : (_e->index() + 1)); }
	
private:
	InsertionPoint(Entity* _context, int _index = UndefinedIndex): m_parent(_context), m_index(_index) {}

	SafePointer<Entity>					m_parent;
	int									m_index;
};

static const InsertionPoint Nowhere;

}
