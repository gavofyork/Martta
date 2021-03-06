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

#include "ReferencedValue.h"

#ifndef M_API_Statements
#define M_API_Statements M_OUTAPI
#endif

namespace Martta
{

class M_CLASS M_API_Statements ArgumentReferencedValue: public_super ScopedReferencedValue
{
	MARTTA_PROPER(ScopedReferencedValue)
	
public:
	ArgumentReferencedValue(ValueDefiner* _subject = 0): ScopedReferencedValue(_subject) {}

	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e);

	static List<ValueDefiner*>			staticRefPossibilities(Position const& _p);
	virtual List<ValueDefiner*>			refPossibilities(Position const& _p) const { return staticRefPossibilities(_p); }
	virtual List<ValueDefiner*>			possibilities() const { return refPossibilities(over()); }
	virtual EditDelegateFace*			newDelegate(CodeScene* _s) { return new CompletionDelegate<ArgumentReferencedValue, ValueDefiner*>(this, _s); }
};

}
