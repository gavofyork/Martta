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

#include "BuiltinType.h"
#include "OperatorLabel.h"
#include "ConstLabel.h"
#include "Compound.h"
#include "TypeConcept.h"
#include "Argument.h"
#include "MethodOperator.h"

namespace Martta
{

MARTTA_PROPER_CPP(MethodOperator);
	
Operator MethodOperator::id() const
{
	if (!childIs<OperatorLabel>(Identity))
		return Operator();
	return childAs<OperatorLabel>(Identity)->id();
}

int MethodOperator::argumentCount() const
{
	return Super::argumentCount() + ((isComplete() && id().isConfusablePostfix()) ? 1 : 0);
}

Type MethodOperator::argumentType(int _i) const
{
	return (isComplete() && id().isConfusablePostfix() && _i == argumentCount() - 1) ? Type(Int) : Super::argumentType(_i);
}

Kinds MethodOperator::allowedKinds(int _i) const
{
	if (_i == Identity)
		return Kind::of<OperatorLabel>();
	if (_i == 0 && isBinary())
		return Kind::of<Argument>();
	return Super::allowedKinds(_i);
}

}
