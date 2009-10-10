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
#include "MethodOperator.h"

#ifndef M_API_Class
#define M_API_Class M_OUTAPI
#endif

namespace Martta
{

class M_API_Class ArtificialAssignmentOperator: public_super MethodOperator, public_interface Artificial
{
	MARTTA_PROPER(MethodOperator)
	MARTTA_ALSO_INHERITS(Artificial, 0)

protected:
	virtual Operator					id() const { return Operator::Equals; }
	virtual String						name() const { return "operator="; }
	virtual String						codeName() const { return "operator="; }
	virtual String						defineHtml() const { return String::null; }
	virtual	bool						isConst() const { return true; }
	virtual Access						access() const { return Public; }
	virtual int							firstArgumentIndex() const { return 0; }
	virtual int							minRequired(int) const { return 0; }
	virtual Kinds						allowedKinds(int) const { return Kinds(); }
	virtual String						interfaceCode() const { return ""; }
	virtual String						implementationCode() const { return ""; }
	virtual Compound*					body() const { return 0; }
	virtual int							argumentCount() const { return 1; }
	virtual Type						argumentType(int) const;
	virtual String						argumentCodeName(int) const { return "source"; }
	virtual String						argumentName(int) const { return "source"; }
	virtual Type						returns() const;
};

}
