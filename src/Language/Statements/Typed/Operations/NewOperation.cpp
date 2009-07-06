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
#include "BuiltinType.h"
#include "Pointer.h"
#include "Array.h"
#include "NewOperation.h"

namespace Martta
{

MARTTA_OBJECT_CPP(NewOperation);	

bool NewOperation::isValidState() const
{
	AssertNR(isComplete());
	return childAs<TypeEntity>(TheOperand)->canStandAlone();
}
	
String NewOperation::code() const
{
	if (!isComplete())
		return String();
	return parenthesise("new " + childAs<TypeEntity>(TheOperand)->code());
}

Kinds NewOperation::allowedKinds(int _index) const
{
	if (_index == TheOperand)
		return Kind::of<Pointer>(), Kind::of<Array>(), Kind::of<ExplicitType>(), Kind::of<BuiltinType>(), Kind::of<FunctionType>();
	return Super::allowedKinds(_index);
}

Type NewOperation::type() const
{
	if (!isComplete())
		return Type();
	if (Array* a = tryChild<Array>(TheOperand))
		return *a;
	return Type(*childAs<TypeEntity>(TheOperand)).topWith(Pointer());
}

}
