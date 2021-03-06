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

class M_CLASS M_API_Types Array: public_super AddressType
{
	MARTTA_PROPER(AddressType)

public:
	MARTTA_NAMED(Length)

	inline static bool					keyPressedOnPosition(Position const& _p, KeyEvent const* _e) { return simplePositionKeyPressHandler<Array>(_p, _e, "["); }

protected:
	virtual bool						hasDefaultConstructor() const { return true; }
	virtual String						code(String const& _middle) const;
	virtual int							minRequired(int _i) const { return _i == Length ? 1 : Super::minRequired(_i); }
	virtual Kinds						allowedKinds(int _i) const;
	virtual Types						allowedTypes(int _i) const;
	virtual TypeConcept*					newClone() const;
	virtual String						defineHtml() const { return toHtml(child(Original)) + L"<^><span class=\"symbol\">[</span>" + toHtml(child(Length)) + L"<span class=\"symbol\">]</span>"; }
	virtual List<Declaration*>			utilised() const;
};

}
