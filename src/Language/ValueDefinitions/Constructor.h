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

#include "MemberCallable.h"

namespace Martta
{

class Class;

class Constructor: public MemberCallable
{
	MARTTA_OBJECT(MemberCallable)

public:
	Class*								classType() const;
	
	static bool							keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e);

protected:
	virtual QString						defineMemberLayout(ViewKeys& _viewKeys) const;
	virtual QString						name() const;
	virtual QString						codeName() const;
	virtual bool						isConst() const { return false; }
	virtual int							firstArgumentIndex() const { return 2; }
	virtual Kinds						allowedKinds(int _i) const;
	virtual bool						isInValidState() const;
	virtual Type						returns() const;
	virtual QString						code(FunctionCodeScope _ref) const;
	virtual Compound*					body() const { return entityAs<Compound>(0); }
};

}
