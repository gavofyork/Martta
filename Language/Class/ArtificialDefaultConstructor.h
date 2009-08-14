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

#include "Artificial.h"
#include "Constructor.h"

namespace Martta
{

class ArtificialDefaultConstructor: public Constructor, public_interface Artificial
{
	MARTTA_OBJECT(Constructor)
	MARTTA_INHERITS(Artificial, 0)

protected:
	virtual int							minRequired(int) const { return 0; }
	virtual Kinds						allowedKinds(int) const { return Kinds(); }
	virtual String						interfaceCode() const { return ""; }
	virtual String						implementationCode() const { return ""; }
	virtual String						defineHtml() const { return String::null; }

	virtual Access						access() const { return Public; }
	virtual Compound*					body() const { return 0; }
	virtual int							argumentCount() const { return 0; }
};

}
