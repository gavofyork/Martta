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

#include "Common.h"
#include "Declaration.h"
#include "ModelPtrRegistrar.h"
#include "ModelPtr.h"

namespace Martta
{

void ModelPtrFace::tryRestore(Declaration const* _root)
{
	if (isArchived())
	{
		m_cache = ModelPtrRegistrar::get()->find(m_key);
		if (!m_cache)
			m_cache = _root->findEntity(m_key);
		// Note; m_cache is allowed to be zero, since it just means that the entity we're
		// pointing at was deleted while we were archived. What we get back is naturally a
		// null pointer.
		if (m_cache)
			m_key = String::null;
	}
}

String ModelPtrFace::key() const
{
	if (!m_key.isEmpty())
		return m_key;
	if (m_cache)
		return m_cache->key();
	return String::null;
}

void ModelPtrFace::set(Identifiable* _e)
{
	m_cache = _e;
	m_key = String::null;
}

void ModelPtrFace::set(String const& _k)
{
	m_cache = 0;
	m_key = _k;
	if (!_k.isEmpty())
		ModelPtrRegistrar::get()->toBeRestored(this);
}

}
