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

MARTTA_OBJECT_CPP(ReferenceOperation);	
	
QString ReferenceOperation::code() const
{
	if (!haveOperand())
		return "";
	return parenthesise("&" + operand()->code());
}

Types ReferenceOperation::allowedTypes(int) const
{
	Types ret;
	foreach (Type t, BareTyped::allowedTypes())
		if (t->isType<AddressType>())
			ret << Type(*t->asType<AddressType>()->childType()).topWith(Reference());
		else
			ret << Type(Void).topWith(Const()).topWith(Reference());
	return ret;
}

Types ReferenceOperation::deniedTypes(int) const
{
	Types ret;
	foreach (Type t, BareTyped::deniedTypes())
		if (t->isType<AddressType>())
			ret << Type(*t->asType<AddressType>()->childType()).topWith(Reference());
	return ret;
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
		p = *p->asType<Reference>()->childType();
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
		p = *p->asType<Reference>()->childType();
	else
		p = Type();

	return p.topWith(Pointer());
}

}
