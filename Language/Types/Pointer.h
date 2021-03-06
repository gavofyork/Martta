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

#include "AddressType.h"

#ifndef M_API_Types
#define M_API_Types M_OUTAPI
#endif

namespace Martta
{

class M_CLASS M_API_Types Pointer: public_super AddressType
{
	MARTTA_PROPER(AddressType)

public:
	static bool keyPressedOnPosition(Position const& _p, KeyEvent const* _e);

protected:
	virtual Types						assignableTypes() const;
	virtual bool						hasDefaultConstructor() const { return true; }
	virtual String						code(String const& _middle) const { return original()->code("*" + _middle); }
	virtual String						modifierHtml() const { return L"<span class=\"symbol\">*</span>"; }
	virtual Rgb							idColour() const { return (original() ? original()->idColour() : TypeConcept::null->idColour()).interpolated(20); }
	virtual TypeConcept*				newClone() const { return new Pointer; }
	virtual List<Declaration*>			utilised() const { return List<Declaration*>(); }
};

}
