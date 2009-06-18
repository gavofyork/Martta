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

MARTTA_OBJECT_CPP(DereferenceOperation);	
	
QString DereferenceOperation::code() const
{
	if (!haveOperand()) return "";
	return parenthesise("*" + operand()->code());
}

Types DereferenceOperation::allowedTypes(int) const
{
	Types ret;
	foreach (Type t, BareTyped::allowedTypes())
		ret << t.topWith(AddressType());
	return ret;
}

Types DereferenceOperation::deniedTypes(int) const
{
	Types ret;
	foreach (Type t, BareTyped::deniedTypes())
		ret << t.topWith(AddressType());
	return ret;
}

Type DereferenceOperation::apparentType() const
{
	// TODO: handle classes whose operator*() is overloaded.
	if (!haveOperand())
		return Type();
	
	Type p(effectiveOperandType());
		   
	// Disguard reference if there is one.
	if (p->isType<Reference>())
		p = *p->asType<Reference>()->childType();

	// Disguard pointer.
	if (p->isKind<AddressType>())
		p = *p->childAs<TypeEntity>(0);
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
		p = *p->asType<Reference>()->childType();

	// Disguard pointer.
	if (p->isKind<AddressType>())
		p = *p->childAs<TypeEntity>(0);
	else
		p = Type();

	return p.topWith(Reference());
}

}
