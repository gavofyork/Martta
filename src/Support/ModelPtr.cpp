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

#include <QtCore>

#include "Common.h"
#include "DeclarationEntity.h"
#include "RootEntity.h"
#include "ModelPtr.h"

namespace Martta
{

void ModelPtrFace::archive()
{
	if (!isArchived() && m_cache)
	{
		m_key = m_cache->key();
		m_rootEntity = m_cache->rootEntity();
	}
}

void ModelPtrFace::restore()
{
	if (isArchived())
	{
		M_ASSERT(!m_cache || !m_cache->context());
		m_cache = m_rootEntity->findEntity(m_key);
		// Note; m_cache is allowed to be zero, since it just means that the entity we're
		// pointing at was deleted while we were archived. What we get back is naturally a
		// null pointer.
		if (!m_cache)
		{
			qCritical() << "Couldn't find" << m_key;
//			m_rootEntity->findEntity(m_key);
		}
		else
		{
			m_rootEntity = 0;
			m_key = QString();
		}
	}
}

void ModelPtrFace::gone(DeclarationEntity* _e)
{
	if (m_cache == _e)
		set(0);
}

DeclarationEntity* ModelPtrFace::get()
{
	return m_cache;
}

QString ModelPtrFace::key() const
{
	if (m_rootEntity) return m_key;
	if (m_cache) return m_cache->key();
	return QString();
}

void ModelPtrFace::set(DeclarationEntity* _e, QString const& _k, RootEntity* _r)
{
	if (m_cache) m_cache->rootEntity()->removeModelPtr(this);
	else if (m_rootEntity) m_rootEntity->removeModelPtr(this);

	m_cache = _e;
	m_rootEntity = _r;
	m_key = _k;

	if (!m_cache && m_rootEntity)
	{
		m_cache = m_rootEntity->findEntity(m_key);
		if (m_cache)
		{
			m_rootEntity = 0;
			m_key = QString();
		}
	}

	if (m_cache) m_cache->rootEntity()->addModelPtr(this);
	else if (m_rootEntity) m_rootEntity->addModelPtr(this);
}

}
