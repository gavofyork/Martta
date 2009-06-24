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

#include <QList>

#include "ChildValidifier.h"

namespace Martta
{

class Type;
typedef QList<Type> Types;

class TypedOwner: public_interface ChildValidifier
{
	MARTTA_INTERFACE
	MARTTA_INHERITS(ChildValidifier, 0)

public:
	virtual Types						allowedTypes(int) const;
	virtual Types						deniedTypes(int) const;
	
	// @returns the type that entity _i effectively has in this place. This takes into effect implicit conversion.
	Type								effectiveType(int _i) const;
	Type								nominalType(int _i) const;
	
protected:
	virtual bool						isChildInValidState(int _i) const;
};

}
