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
class Root: public Declaration
{
	MARTTA_OBJECT(Declaration)

public:
	virtual String						name() const { return String(); }
	virtual Entity*						parent() const { return 0; }
	virtual String						reference() const { return ""; }
	virtual String						key() const { return ""; }
	virtual Kinds						allowedKinds(int) const;
};

}
