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
#include "Const.h"
#include "Pointer.h"
#include "ReferenceOperation.h"

namespace Martta
{

MARTTA_PROPER_CPP(ReferenceOperation);

String ReferenceOperation::code() const
{
	if (!haveOperand())
		return "";
	return parenthesise(L"&" + operand()->code());
}

Types ReferenceOperation::allowedTypes(int _i) const
{
	if (_i == TheOperand)
	{
		Types ret;
		foreach (Type t, BareTyped::ourAllowedTypes())
			if (t->isType<AddressType>())
				ret << Type(*t->asType<AddressType>()->original()).topWith(Reference());
			else
			{
				ret << Type().topWith(Reference());
//				ret << Type(Void).topWith(Const()).topWith(Reference());
			}
		return ret;
	}
	return Super::allowedTypes(_i);
}

Types ReferenceOperation::deniedTypes(int _i) const
{
	if (_i == TheOperand)
	{
		Types ret;
		foreach (Type t, BareTyped::ourDeniedTypes())
			if (t->isType<AddressType>())
				ret << Type(*t->asType<AddressType>()->original()).topWith(Reference());
		return ret;
	}
	return Super::deniedTypes(_i);
}

Type ReferenceOperation::apparentType() const
{
	// TODO: handle classes whose operator*() is overloaded.
	if (!haveOperand())
		return Type();
	// Copy tree of the operand's type.
	Type p(effectiveOperandType());

	// Disguard reference if there is one.
	if (p->isType<Reference>())
		p = *p->asType<Reference>()->original();
	else
		p = Type();

	return p.topWith(Pointer());
}

Type ReferenceOperation::type() const
{
	// TODO: handle classes whose operator*() is overloaded.
	if (!haveOperand())
		return Type();
	// Copy tree of the operand's type.
	Type p(operandType());

	// Disguard reference if there is one.
	if (p->isType<Reference>())
		p = *p->asType<Reference>()->original();
	else
		p = Type();

	return p.topWith(Pointer());
}

}
