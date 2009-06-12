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

namespace Martta
{

class Class;

class Destructor: public MemberLambda
{
	MARTTA_OBJECT(MemberLambda)

public:
	static bool							keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e);

protected:
	virtual int							memberMinimumRequired() const { return 1; }
	virtual Kinds						memberAllowedKinds(int _i) const;
	virtual QString						memberDefineLayout(ViewKeys& _viewKeys) const;
	virtual bool						isConst() const { return false; }
	virtual bool						isInValidState() const { return Super::Super::isInValidState(); }

	virtual QString						name() const;
	virtual QString						codeName() const;
	
	virtual Type						returns() const { return Type(); }
	virtual QString						basicCode(FunctionCodeScope _ref) const;
};

}
