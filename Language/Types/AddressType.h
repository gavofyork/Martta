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

#include "Subscriptable.h"
#include "ModifyingType.h"

namespace Martta
{

class AddressType: public ModifyingType, public_interface Subscriptable
{
	MARTTA_INITIALISED_PLACEHOLDER(ModifyingType)
	MARTTA_INHERITS(Subscriptable, 0)

public:
	static void							initialiseClass();
	static void							finaliseClass();

private:
	virtual TypeEntity*					newClone() const { return new AddressType; }
	virtual bool						defineSimilarityTo(TypeEntity const* _t, Castability _c) const;
	virtual String						modifierHtml() const { return L"<span class=\"symbol\">@</span>"; }
	virtual String						code(String const& _middle) const { return original()->code("@" + _middle); }
	virtual Types						subscriptTypes() const;
	virtual Type						subscriptsTo(Type const&) const;
};

}
