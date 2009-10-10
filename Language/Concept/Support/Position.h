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
#include <msTextStream.h>
using namespace MarttaSupport;

#include "SafePointer.h"
#include "Kind.h"

#ifndef M_API_Concept
#define M_API_Concept M_OUTAPI
#endif

namespace Martta
{

static const int UndefinedIndex = INT_MAX;

class Concept;

class M_API_Concept Position
{
	friend class Concept;
	friend TextStream& operator<<(TextStream& _out, Position const& _item);

public:
	Position(): m_parent(0), m_index(UndefinedIndex) {}
	Position(Position const& _c): m_parent(_c.m_parent), m_index(_c.m_index) {}
	Position& operator=(Position const& _c) { m_parent = _c.m_parent; m_index = _c.m_index; return *this; }

	inline bool							operator==(Position const& _c) const { return _c.m_parent == m_parent && _c.m_index == m_index; }
	inline bool							operator!=(Position const& _c) const { return !operator==(_c); }

	inline 								operator bool() const { return isValid(); }
	template<class T> inline bool		allowedToBeKind() const { return allowedToBeKind(Kind::of<T>()); }
	bool								allowedToBeKind(Kind _k) const;
	Kinds								allowedKinds() const;
	Kinds								deniedKinds() const;
	bool								isRequired() const;
	bool								isFixed() const;
	Concept*								nearestConcept() const;

	Concept&								operator*() const { return *entity(); }
	Concept*								operator->() const { return entity(); }
	bool								exists() const;
	Concept*								entity() const;

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
	Concept*								place(Concept* _e) const;

	/**
	 * Inserts @a _e into the parent's entity list, so it becomes at the position
	 * required.
	 *
	 * Ignores the placeholders (if any) that might exist already at that position.
	 */
	Concept*								insert(Concept* _e) const;

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
	void								insertSilent(Concept* _e) const;

	inline Concept*						parent() const { return m_parent; }
	inline int							index() const { return m_index; }

	/**
	 * This will not call changed() as appropriate.
	 */
	Concept*								spawnPreparedSilent() const;

	/**
	 * This will call changed() as appropriate.
	 */
	Concept*								spawnPrepared() const;


	// TODO: Bring these in once I'm comfortable with them.
//	template<class T> inline T*			operator<<=(T* _e) const { place(_e); return _e; }
//	template<class T> inline Position const& operator<<(T* _e) const { place(_e); return Position(m_parent, (m_index == -1) ? -1 : (_e->index() + 1)); }

private:
	Position(Concept* _context, int _index = UndefinedIndex): m_parent(_context), m_index(_index) {}

	SafePointer<Concept>					m_parent;
	int									m_index;
};

static const Position Nowhere;

}
