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

#include "Const.h"
#include "Reference.h"
#include "AssignmentOperation.h"

namespace Martta
{

MARTTA_OBJECT_CPP(AssignmentOperation);	
	
QString AssignmentOperation::code() const
{
	Typed* e = asTyped(FirstOperand);
	Typed* f = asTyped(SecondOperand);
	if (!e || !f) return "";
	return parenthesise(e->code() + " = " + f->code());
}

Types AssignmentOperation::allowedTypes(int _index) const
{
	if (_index == FirstOperand)
		return Type().topWith(Reference());
	if (_index == SecondOperand && asTyped(FirstOperand) && typeOf(FirstOperand)->isType<Reference>())
	{
		return typeOf(FirstOperand)->asType<Reference>()->childType()->assignableTypes();
	}
	return Types();
}

Type AssignmentOperation::type() const
{
	// TODO: Handle types whose "=" is overloaded.
	return typeOf(FirstOperand);
}

}
