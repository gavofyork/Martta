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

#include "Entity.h"
#include "ModelPtrFace.h"
#include "ModelPtrRegistrar.h"

namespace Martta
{

ModelPtrRegistrar* ModelPtrRegistrar::s_this = 0;

void ModelPtrRegistrar::toBeRestored(ModelPtrFace* _p)
{
	m_modelPtrs << _p;
}

void regDecs(Identifiable* _d)
{
	ModelPtrRegistrar::get()->registerTemp(_d, _d->key());
	foreach (Identifiable* i, _d->self()->childrenOf<Identifiable>())
		regDecs(i);
}

void ModelPtrRegistrar::restorePtrs(Identifiable const* _root)
{
	mInfo() << "Restoring up to" << m_modelPtrs.size() << "pointers";
	
	foreach (Identifiable* d, _root->self()->childrenOf<Identifiable>())
		regDecs(d);
	
	// NOTE: Some model pointers naturally depend on others further down the model pointer list to be understood and restored.
	// For this reason we loop doing multiple passes until no more model pointers can be restored. This results in a worst-case
	// performance of O(n^2), which given the vast amount of model pointers most programs entail is unacceptable.
	// This should be changed to a JIT system whereby model pointers are resolved when needed, and thus when the list is resolved
	// any dependencies are worked through appropriately. Having thought about it, I'm reasonably confident that the spectre of
	// circular dependencies will not arise, if only because this thing is possible and there's no reason why either of the methods
	// given for restoration are flawed.
	int restored = 0;
	for (List<ModelPtrFace*>::Iterator i = m_modelPtrs.begin(); i != m_modelPtrs.end();)
	{
		(*i)->tryRestore(_root);
		if ((*i)->isArchived())
			++i;
		else
		{
			restored++;
			i = m_modelPtrs.erase(i);
		}
	}
		
	mInfo() << "Restored " << restored << "pointers";
	
	m_tempRegistered.clear();
	
	M_FOREACH (ModelPtrFace* i, m_modelPtrs)
		mCritical() << "ERROR: Couldn't restore model pointer with key: " << i->key();
}

}

