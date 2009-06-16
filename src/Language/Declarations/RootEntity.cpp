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

#include "TopLevelType.h"
#include "RootEntity.h"

namespace Martta
{

RootEntity* RootEntity::s_this = 0;

MARTTA_OBJECT_CPP(RootEntity);	
	
RootEntity::RootEntity():
	m_archivalState	(Restored)
{
	m_rootEntity = this;
	s_this = this;
}

RootEntity::~RootEntity()
{
	clearEntities();
}

Kinds RootEntity::allowedKinds(int) const
{
	return Kinds() << Kind::of<TopLevel>();
}

void RootEntity::doCulling()
{
	while (m_cullList.size())
		if (Entity* e = m_cullList.takeLast())
			e->cull();
}

void RootEntity::setChanged()
{
	if (!m_changed)
		QTimer::singleShot(0, this, SLOT(ensureSyncedModel()));
	m_changed = true;
}

void RootEntity::ensureSyncedModel()
{
	emit modelChanged();
	m_changed = false;
}

void RootEntity::noteDeletion(Identifiable* _e)
{
	foreach (ModelPtrFace* i, m_modelPtrs)
		i->gone(_e);
	setChanged();
}

void RootEntity::addModelPtr(ModelPtrFace* _p)
{
	m_modelPtrs << _p;
}

void RootEntity::removeModelPtr(ModelPtrFace* _p)
{
	m_modelPtrs.removeAll(_p);
}

void RootEntity::archivePtrs(bool _clear) const
{
	m_archivalState = Archiving;
	qInformation() << "Archiving" << m_modelPtrs.size() << "pointers";
	foreach(ModelPtrFace* i, m_modelPtrs)
		i->archive();
	if (_clear)
		foreach(ModelPtrFace* i, m_modelPtrs)
			i->clearCache();
	m_archivalState = Archived;
}

void RootEntity::restorePtrs() const
{
	m_archivalState = Restoring;
	qInformation() << "Restoring" << m_modelPtrs.size() << "pointers";
	// NOTE: Some model pointers naturally depend on others further down the model pointer list to be understood and restored.
	// For this reason we loop doing multiple passes until no more model pointers can be restored. This results in a worst-case
	// performance of O(n^2), which given the vast amount of model pointers most programs entail is unacceptable.
	// This should be changed to a JIT system whereby model pointers are resolved when needed, and thus when the list is resolved
	// any dependencies are worked through appropriately. Having thought about it, I'm reasonably confident that the spectre of
	// circular dependencies will not arise, if only because this thing is possible and there's no reason why either of the methods
	// given for restoration are flawed.
	for (int restored = 0;; restored = 0)
	{
		foreach(ModelPtrFace* i, m_modelPtrs)
			if (i->isArchived())
			{
				i->tryRestore();
				if (!i->isArchived())
					restored++;
			}
		if (!restored) break;
	}
	
	foreach(ModelPtrFace* i, m_modelPtrs)
		if (i->isArchived())
		{
			qCritical() << "ERROR: Couldn't restore model pointer with key: " << i->key();
			i->tryRestore();
			i->gone(0);
		}
	m_archivalState = Restored;
}

}
