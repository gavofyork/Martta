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

#include "ValueDefiner.h"
#include "Member.h"

#ifndef M_API_Members
#define M_API_Members M_OUTAPI
#endif

namespace Martta
{

class M_CLASS M_API_Members MemberValue: public_super Member, public_interface ValueDefiner
{
	MARTTA_PLACEHOLDER(Member)
	MARTTA_ALSO_INHERITS(ValueDefiner, 0)

public:
	virtual bool						isConst() const { return true; }

protected:
	virtual String						nonSpecificReference() const { return codeName(); }
	virtual	bool						isAccessibleAt(Position const& _p) const;
	Type								memberifiedType(Type const& _t) const;
	virtual void						apresLoad() { Member::apresLoad(); ValueDefiner::apresLoad(); }
};

}
