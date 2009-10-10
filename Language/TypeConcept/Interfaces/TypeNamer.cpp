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

#include "TypedOwner.h"
#include "TypeNamer.h"

namespace Martta
{

MARTTA_INTERFACE_CPP(TypeNamer)

Types TypeNamer::ourAllowedTypes() const
{
	if (!self()->parentIs<TypedOwner>())
		return Types();
	return self()->parentAs<TypedOwner>()->allowedTypes(self()->index());
}

Types TypeNamer::ourDeniedTypes() const
{
	if (!self()->parentIs<TypedOwner>())
		return Types();
	return self()->parentAs<TypedOwner>()->deniedTypes(self()->index());
}

String TypeNamer::informationHtml() const
{
	Pairs p("Concept Type", true);
	p << L"Canonical type" << type()->name();
	if (apparentType() != type())
		p << L"Apparent type" << apparentType()->name();
	if (ourAllowedTypes().count())
		p << L"Allowed types" << compileTypes(ourAllowedTypes());
	if (ourDeniedTypes().count())
		p << L"Denied types" << compileTypes(ourDeniedTypes());
	return p;
}

}
