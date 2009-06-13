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

#include "Typed.h"
#include "Function.h"
#include "ReturnStatement.h"
#include "Type.h"

namespace Martta
{

MARTTA_OBJECT_CPP(ReturnStatement);	
	
int ReturnStatement::minimumRequired() const
{
	if (!hasAncestor<LambdaNamer>())
	{
		qCritical("*** ERROR: Return statement without callable ancestor!");
		return 0;
	}

	if (ancestor<LambdaNamer>()->returns().isNull() || ancestor<LambdaNamer>()->returns() == Type(Void))
		return 0;
	else
		return 1;
}

void ReturnStatement::onDependencyChanged(Entity*)
{
	if (validifyChildren())
		relayoutLater();
}

Kinds ReturnStatement::allowedKinds(int _i) const
{
	if (!hasAncestor<LambdaNamer>())
	{
		qCritical("Return statement without lambda ancestor!");
		return Kinds();
	}

	if (_i == 0 && !ancestor<LambdaNamer>()->returns().isNull() && ancestor<LambdaNamer>()->returns() != Type(Void))
		return Kind::of<Typed>();
	return Kinds();
}

Types ReturnStatement::allowedTypes(int _i) const
{
	if (!hasAncestor<LambdaNamer>())
	{
		qCritical("Return statement without lambda ancestor!");
		return Types();
	}
	if (_i == 0 && !ancestor<LambdaNamer>()->returns().isNull() && ancestor<LambdaNamer>()->returns() != Type(Void))
		return ancestor<LambdaNamer>()->returns();
	return Types();
}

QString ReturnStatement::code() const
{
	return entity(0) ? "return " + entityAs<Typed>(0)->codeAsStatement() : "return;";
}

}
