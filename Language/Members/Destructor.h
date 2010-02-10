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

#include "MemberLambda.h"

#ifndef M_API_Members
#define M_API_Members M_OUTAPI
#endif

namespace Martta
{

class Class;

class M_CLASS M_API_Members Destructor: public_super MemberLambda
{
	MARTTA_PROPER(MemberLambda)

public:
	static bool							keyPressedOnPosition(Position const& _p, KeyEvent const* _e);

protected:
	virtual int							minRequired(int _i) const { return _i == Identity || _i == Returned || _i == Constness ? 0 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int _i) const;
	virtual String						defineNameHtml() const { return L"<span class=\"symbol\">~</span>" + typeDefinition()->labelHtml(typeDefinition()->nick()); }
	virtual String						defineReturnHtml() const { return String::null; }
	virtual bool						isConst() const { return false; }
	virtual bool						isInValidState() const { return Super::Super::isInValidState(); }

	virtual String						nick() const;
	virtual String						codeName() const;

	virtual Type						returns() const { return Type(); }
	virtual String						basicCode(FunctionCodeScope _ref) const;
};

}
