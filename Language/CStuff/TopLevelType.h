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

#include "TypeDefinition.h"
#include "NameEntryPoint.h"
#include "TopLevel.h"

#ifndef M_API_CStuff
#define M_API_CStuff M_OUTAPI
#endif

namespace Martta
{

class M_CLASS M_API_CStuff TopLevelType: public_super TopLevel, public_interface TypeDefinition, public_interface NameEntryPoint
{
	MARTTA_PLACEHOLDER(TopLevel)
	MARTTA_ALSO_INHERITS(TypeDefinition, 0)
	MARTTA_ALSO_INHERITS(NameEntryPoint, 1)

protected:
	// From SubAddressable
	virtual int							familyDependencies() const { return DependsOnChildren; }

	// From TypeDefinition
	virtual String						code() const { return codeName(); }
	virtual String						summary() const { return code(); }
	virtual bool						hasDefaultConstructor() const { return false; }
	virtual Types						assignableTypes() const;
	virtual List<Declaration*>			utilisedInUse() const { return List<Declaration*>() << const_cast<TopLevelType*>(this); }
	virtual void						apresLoad() { TopLevel::apresLoad(); TypeDefinition::apresLoad(); }
	virtual bool						keyPressed(KeyEvent const* _e) { return NameEntryPoint::keyPressed(_e) || Super::keyPressed(_e); }
};

}
