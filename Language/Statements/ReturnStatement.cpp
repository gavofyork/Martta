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

#include <msDebug.h>
using namespace MarttaSupport;

#include "IdentifierSet.h"
#include "Typed.h"
#include "Type.h"
#include "BuiltinType.h"
#include "ReturnStatement.h"

namespace Martta
{

MARTTA_PROPER_CPP(ReturnStatement);
MARTTA_NAMED_CPP(ReturnStatement, Returned);

static SimpleIdentifierSet<ReturnStatement> s_returnStatementSet(L"return");

int ReturnStatement::minRequired(int _i) const
{
	if (!parent())
		return 0;

	if (!hasAncestor<LambdaNamer>())
	{
		mCritical() << "*** ERROR: Return statement without callable ancestor!";
		debugTree();
		return 0;
	}

	if (_i == Returned)
		if (ancestor<LambdaNamer>()->returns().isNull() || ancestor<LambdaNamer>()->returns() != Type(Void))
			return 1;
		else
			return 0;
	else
		return Super::minRequired(_i);
}

void ReturnStatement::onDependencyChanged(int, Concept*)
{
	validifyChildren();
	changed(Logically);
}

Kinds ReturnStatement::allowedKinds(int _i) const
{
	if (!hasAncestor<LambdaNamer>())
	{
		mCritical() << "Return statement without lambda ancestor!";
		return Kinds();
	}

	if (_i == Returned && (ancestor<LambdaNamer>()->returns().isNull() || ancestor<LambdaNamer>()->returns() != Type(Void)))
		return Kind::of<Typed>();
	return Super::allowedKinds(_i);
}

Types ReturnStatement::allowedTypes(int _i) const
{
	if (_i == Returned)
	{
		if (!hasAncestor<LambdaNamer>())
		{
			mCritical() << "Return statement without lambda ancestor!";
			return Types();
		}
		if (ancestor<LambdaNamer>()->returns() == Type(Void))
			return Types();
		if (ancestor<LambdaNamer>()->returns().isNull())
			return Type();
		return ancestor<LambdaNamer>()->returns();
	}
	return Super::allowedTypes(_i);
}

String ReturnStatement::code() const
{
	return childIs<Typed>(Returned) ? "return " + childAs<Typed>(Returned)->codeAsStatement() : "return;";
}

}
