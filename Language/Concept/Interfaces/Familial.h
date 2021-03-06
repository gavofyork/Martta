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

#include "Meta.h"

#ifndef M_API_Concept
#define M_API_Concept M_OUTAPI
#endif

namespace Martta
{

class M_API_Concept Familial
{

public:
	virtual void						childrenInitialised() {}
	virtual void						childAdded(int _newChildsIndex) { (void)(_newChildsIndex); }
	virtual void						childSwitched(Concept* _currentChild, Concept* _exChild) { (void)(_currentChild); (void)(_exChild); }
	virtual void						childRemoved(Concept* _exChild, int _exChildsIndex) { (void)(_exChild); (void)(_exChildsIndex); }
	virtual void						childMoved(Concept* _child, int _originalIndex) { (void)(_child); (void)(_originalIndex); }
	virtual void						parentAdded() {}
	virtual void						parentSwitched(Concept* _exParent) { (void)(_exParent); }
	virtual void						parentRemoved(Concept* _exParent) { (void)(_exParent); }

	virtual ~Familial() {}
};

}
