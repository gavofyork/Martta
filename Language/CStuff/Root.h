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

#include "Declaration.h"

#ifndef M_API_CStuff
#define M_API_CStuff M_OUTAPI
#endif

namespace Martta
{

/**
 * The language root for an entire program.
 *
 * This encompasses everything required to compile a single program. It should include forward declarations of all
 * kinds from all dependencies (including C-style).
 *
 * Aside from that it should also encompass all the implementation of the project in question.
 */
class M_API_CStuff Root: public_super Declaration
{
	MARTTA_PROPER(Declaration)

public:
	MARTTA_NAMED(Included)

	virtual String						name() const { return String::null; }
	virtual Concept*					parent() const { return 0; }
	virtual String						reference() const { return ""; }
	virtual String						key() const { return ""; }
	virtual Kinds						allowedKinds(int) const;
	virtual void						apresLoad() { restorePtrs(); }		// XXX: I don't think this is needed...
	virtual void						properties(Hash<String, String>& _p) const { Declaration::Super::properties(_p); }
	virtual void						setProperties(Hash<String, String> const& _p) { Declaration::Super::setProperties(_p); }
};

}
