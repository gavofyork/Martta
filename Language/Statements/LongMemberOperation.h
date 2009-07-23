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

#include "GenericMemberOperation.h"

namespace Martta
{

class LongMemberOperation: public GenericMemberOperation
{
	MARTTA_OBJECT(GenericMemberOperation)

public:
	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e);

protected:
	virtual Type						scope() const;
	virtual Types						allowedTypes(int _index) const;
	virtual Operator					id() const { return Operator::XArrow; }
	virtual String						code() const;
	virtual String						defineLayout(ViewKeys const&) const { return String("%1;Mi;^;p:/arrow.svg;Mi;%2").arg(FirstOperand).arg(SecondOperand); }
};

}