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

#include "UnaryOperation.h"

#ifndef M_API_Statements
#define M_API_Statements M_OUTAPI
#endif

namespace Martta
{

class M_CLASS M_API_Statements DereferenceOperation: public_super UnaryOperation
{
	MARTTA_PROPER(UnaryOperation)

public:
	inline static bool					keyPressedOnPosition(Position const& _p, KeyEvent const* _e) { return simpleKeyPressedOnPositionHandler<DereferenceOperation>(_p, _e, Operator::StarX); }

private:
	virtual Types						allowedTypes(int) const;
	virtual Types						deniedTypes(int) const;
	virtual Type						apparentType() const;
	virtual Type						type() const;
	virtual String						code() const;
	virtual Operator					id() const { return Operator::StarX; }
};

}
