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
#include "ConstLabel.h"
#include "Compound.h"
#include "TypeEntity.h"
#include "Variable.h"
#include "MethodOperator.h"

namespace Martta
{

MARTTA_OBJECT_CPP(MethodOperator);
	
Operator MethodOperator::id() const
{
	if (!entityIs<OperatorLabel>(0))
		return Operator();
	return entityAs<OperatorLabel>(0)->id();
}

int MethodOperator::argumentCount() const
{
	return Super::argumentCount() + ((isComplete() && id().isConfusablePostfix()) ? 1 : 0);
}

Type MethodOperator::argumentType(int _i) const
{
	return (isComplete() && id().isConfusablePostfix() && _i == argumentCount() - 1) ? Type(Int) : Super::argumentType(_i);
}

Kinds MethodOperator::memberAllowedKinds(int _i) const
{
	if (_i == 0)
		return Kind::of<OperatorLabel>();
	if (_i == 1)
		return Kind::of<Compound>();
	if (_i == 2)
		return Kind::of<TypeEntity>();
	if (_i == 3)
		return Kind::of<ConstLabel>();
	if (_i == 4 && isBinary())
		return Kind::of<Variable>();
	return Kinds();
}

}
