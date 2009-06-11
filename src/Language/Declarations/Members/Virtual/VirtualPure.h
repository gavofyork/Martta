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

#include "VirtualMethod.h"

namespace Martta
{

// Only has ReturnType, ArgumentVariable...
class VirtualPure: public VirtualMethod
{
	MARTTA_OBJECT(VirtualMethod)

public:
	static bool							keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e);
	
protected:
	virtual QString						interfaceCode() const;
	virtual QString						implementationCode() const { return ""; }
	virtual int							firstArgumentIndex() const { return 4; }
	virtual Kinds						allowedKinds(int _i) const;
	virtual inline Type					returns() const { return *entityAs<TypeEntity>(1); }
	virtual Compound*					body() const { return 0; }
	virtual QString						defineMemberLayout(ViewKeys&) const { return "^;ycode;'virtual';Mi;1;Mo;>name;0;'(';" + times(4, entityCount(), ";',';") + ";')';Mo;3;Mo;'= 0;'"; }
};

}
