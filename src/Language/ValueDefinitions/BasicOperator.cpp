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

#include "OperatorLabel.h"
#include "Compound.h"
#include "TypeEntity.h"
#include "Variable.h"
#include "BasicOperator.h"

namespace Martta
{

MARTTA_OBJECT_CPP(BasicOperator);

Operator BasicOperator::id() const
{
	if (!isComplete())
		return Operator();
	return entityAs<OperatorLabel>(0)->id();
}

Entity* BasicOperator::isExpander() const
{
	return body()->entity(0);
}

int BasicOperator::argumentCount() const
{
	return LambdaNamer::argumentCount() + ((isComplete() && id().isConfusablePostfix()) ? 1 : 0);
}

Type BasicOperator::argumentType(int _i) const
{
	return (isComplete() && id().isConfusablePostfix() && _i == LambdaNamer::argumentCount() - 1) ? Type(Int) : LambdaNamer::argumentType(_i);
}

Kinds BasicOperator::allowedKinds(int _i) const
{
	if (_i == 0)
		return Kind::of<OperatorLabel>();
	if (_i == 1)
		return Kind::of<Compound>();
	if (_i == 2)
		return Kind::of<TypeEntity>();
	if (_i < minimumRequired())
		return Kind::of<Variable>();
	return Kinds();
}

int BasicOperator::minimumRequired() const
{
	if (isBinary())
		return 5;
	return 4;
}

bool BasicOperator::isBinary() const
{
	if (!entityIs<OperatorLabel>(0))
		return false;
	Operator o = entityAs<OperatorLabel>(0)->id();
	if (o.isBinary())
		return true;
	return false;
}

}
