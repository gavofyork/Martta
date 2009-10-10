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

#include "Reference.h"
#include "Pointer.h"
#include "Const.h"
#include "DereferenceOperation.h"

namespace Martta
{

MARTTA_PROPER_CPP(DereferenceOperation);	
	
String DereferenceOperation::code() const
{
	if (!haveOperand())
		return "";
	return parenthesise("*" + operand()->code());
}

Types DereferenceOperation::allowedTypes(int _i) const
{
	if (_i == TheOperand)
	{
		Types ret;
		foreach (Type t, ourAllowedTypes())
		{
			if (t->isType<Reference>())
				t = *t->asType<Reference>()->original();
			ret << t.topWith(AddressType());
		}
		return ret;
	}
	return Super::allowedTypes(_i);
}

Types DereferenceOperation::deniedTypes(int _i) const
{
	if (_i == TheOperand)
	{
		Types ret;
		foreach (Type t, BareTyped::ourDeniedTypes())
		{
			if (t->isType<Reference>())
				t = *t->asType<Reference>()->original();
			ret << t.topWith(AddressType());
		}
		return ret;
	}
	return Super::deniedTypes(_i);
}

Type DereferenceOperation::apparentType() const
{
	// TODO: handle classes whose operator*() is overloaded.
	if (!haveOperand())
		return Type();
	
	Type p(effectiveOperandType());
		   
	// Disguard reference if there is one.
	if (p->isType<Reference>())
		p = *p->asType<Reference>()->original();

	// Disguard pointer.
	if (p->isKind<AddressType>())
		p = *p->asKind<AddressType>()->original();
	else
		p = Type();

	return p.topWith(Reference());
}

Type DereferenceOperation::type() const
{
	// TODO: handle classes whose operator*() is overloaded.
	if (!haveOperand())
		return Type();
	
	Type p(operandType());
		   
	// Disguard reference if there is one.
	if (p->isType<Reference>())
		p = *p->asType<Reference>()->original();

	// Disguard pointer.
	if (p->isKind<AddressType>())
		p = *p->asKind<AddressType>()->original();
	else
		p = Type();

	return p.topWith(Reference());
}

}
