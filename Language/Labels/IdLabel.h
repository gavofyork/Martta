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

#include "Label.h"

#ifndef M_API_Labels
#define M_API_Labels M_OUTAPI
#endif

namespace Martta
{

class M_API_Labels IdLabel: public Label
{
	MARTTA_PLACEHOLDER(Label)
	
public:
	// Potentially queries the parent's isChildInValidState() to check if we're valid.
	// @note NEVER call them from the parent's isChildInValidState() function.
	virtual String						name() const { return String::null; }
};

}

