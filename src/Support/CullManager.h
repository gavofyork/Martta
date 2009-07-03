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

#include <QObject>
#include <QTimer>

#include "SafePointer.h"
#include "Entity.h"

namespace Martta
{

class CullManager: public QObject
{
	Q_OBJECT
	
public:
	static inline CullManager*			get() { return s_this ? s_this : (s_this = new CullManager); } 

	inline void							checkCull(Entity* _e) { if (!m_cullList.size()) QTimer::singleShot(0, this, SLOT(doCulling())); m_cullList << _e; }
	
private slots:
	void								doCulling();

private:
	// List of entities to check and possibly delete at next opportunity when nothing else happening.
	List<SafePointer<Entity> >			m_cullList;

	static CullManager*					s_this;
};

}
