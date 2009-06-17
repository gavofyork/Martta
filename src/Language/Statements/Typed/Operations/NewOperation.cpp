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

#include "AddressType.h"
#include "ExplicitType.h"
#include "FunctionType.h"
#include "SimpleType.h"
#include "Pointer.h"
#include "Array.h"
#include "NewOperation.h"

namespace Martta
{

MARTTA_OBJECT_CPP(NewOperation);	

bool NewOperation::isValidState() const
{
	M_ASSERT(isComplete());
	return entityAs<TypeEntity>(0)->canStandAlone();
}
	
QString NewOperation::code() const
{
	if (!isComplete())
		return QString();
	return parenthesise("new " + entityAs<TypeEntity>(0)->code());
}

Kinds NewOperation::allowedKinds(int _index) const
{
	if (_index == 0)
		return Kind::of<Pointer>(), Kind::of<Array>(), Kind::of<ExplicitType>(), Kind::of<SimpleType>(), Kind::of<FunctionType>();
	return Super::allowedKinds(_index);
}

Type NewOperation::type() const
{
	if (!isComplete())
		return Type();
	if (entityIs<Array>(0))
		return *entityAs<TypeEntity>(0);
	return Type(*entityAs<TypeEntity>(0)).topWith(Pointer());
}

}
