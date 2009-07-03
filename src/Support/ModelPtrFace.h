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

#include "Common.h"

namespace Martta
{

class Identifiable;
class Declaration;
class ModelPtrRegistrar;

class ModelPtrFace
{
	friend class ModelPtrRegistrar;
	
public:
	inline ModelPtrFace(Identifiable* _e): m_cache(0) { set(_e); }
	inline ModelPtrFace(ModelPtrFace const& _c): m_cache(0) { M_ASSERT(!_c.isArchived()); set(_c.m_cache); }
	inline ~ModelPtrFace() { set(0); }

	inline ModelPtrFace& operator=(ModelPtrFace const& _c) { if (_c.m_cache) set(_c.m_cache); else set(_c.m_key); return *this; }

	inline operator bool() const { return !isNull(); }

	inline bool operator==(ModelPtrFace const& _c) const { return m_cache == _c.m_cache && m_key == _c.m_key; }
	inline bool operator!=(ModelPtrFace const& _c) const { return !operator==(_c); }

	/// Nullity is where the pointer fundamentally doesn't address anything.
	inline bool isNull() const { return !m_cache && m_key.isEmpty(); }
	/// Usability is whether get() will work.
	inline bool isUsable() const { return m_cache; }
	/// Archived is whether, should the object pointed to by get() be deleted, the position in the language tree it
	/// took is recorded for later restoration.
	inline bool isArchived() const { return !m_key.isEmpty(); }

	inline String identity() const { return key().section("::", -1); }
	String key() const;

	void restoreFrom(String const& _key) { set(_key); }
	void tryRestore(Declaration const* _root);

protected:
	void set(Identifiable* _e);
	void set(String const& _k = String::null);
	inline Identifiable* get() { return m_cache; }

private:
	Identifiable*		m_cache;
	String				m_key;
};

}
