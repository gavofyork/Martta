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

#include <QString>

#include "Common.h"

namespace Martta
{

class BasicRoot;
class Identifiable;

class ModelPtrFace
{
public:
	inline ModelPtrFace(Identifiable* _e): m_cache(0), m_rootEntity(0) { set(_e); }
	inline ModelPtrFace(QString const& _k, BasicRoot* _r): m_cache(0), m_rootEntity(0) { set(0, _k, _r); }
	inline ModelPtrFace(ModelPtrFace const& _c): m_cache(0), m_rootEntity(0) { set(_c.m_cache, _c.m_key, _c.m_rootEntity); }
	inline ~ModelPtrFace() { set(0); }

	inline ModelPtrFace& operator=(ModelPtrFace const& _c) { set(_c.m_cache, _c.m_key, _c.m_rootEntity); return *this; }

	inline operator bool() const { return m_cache || m_rootEntity; }

	inline bool operator==(ModelPtrFace const& _c) const { return m_cache == _c.m_cache && m_key == _c.m_key && m_rootEntity == _c.m_rootEntity; }
	inline bool operator!=(ModelPtrFace const& _c) const { return !operator==(_c); }

	/// Nullity is where the pointer fundamentally doesn't address anything.
	inline bool isNull() const { return !m_cache && !m_rootEntity; }
	/// Usability is whether get() will work.
	inline bool isUsable() const { return m_cache; }
	/// Archived is whether, should the object pointed to by get() be deleted, the position in the language tree it
	/// took is recorded for later restoration.
	inline bool isArchived() const { return m_rootEntity; }

	inline QString identity() const { return key().section("::", -1); }
	QString key() const;

	void archive();
	void restore();
	void tryRestore();
	void clearCache() { if (isArchived()) m_cache = 0; }
	void gone(Identifiable* _e);

protected:
	void set(Identifiable* _e, QString const& _k = QString(), BasicRoot* _r = 0);
	Identifiable* get();

private:
	Identifiable*		m_cache;
	QString				m_key;
	BasicRoot*			m_rootEntity;
};

}
