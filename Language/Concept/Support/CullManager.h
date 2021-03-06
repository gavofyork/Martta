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

#include "SafePointer.h"
#include "Concept.h"

#ifndef M_API_Concept
#define M_API_Concept M_OUTAPI
#endif

namespace Martta
{

class M_API_Concept DelayedActor
{
public:
	virtual ~DelayedActor() {}

	virtual void						prime() = 0;
};

class M_API_Concept CullManager
{
public:
	static inline CullManager*			get() { return s_this ? s_this : (s_this = new CullManager); } 
	
	CullManager(): m_actor(0) {}
	virtual ~CullManager() {}

	void								setDelayedActor(DelayedActor* _a = 0) { m_actor = _a; }

	inline void							checkCull(Concept* _e) { if (m_actor) m_actor->prime(); m_cullList << _e; }
	void								doCulling();

private:
	// List of entities to check and possibly delete at next opportunity when nothing else happening.
	List<SafePointer<Concept> >			m_cullList;
	DelayedActor*						m_actor;
	static CullManager*					s_this;
};

}
