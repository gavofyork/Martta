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

#include "ListType.h"
#include "HashType.h"
#include "Type.h"
#include "Reference.h"
#include "Const.h"
#include "AddressType.h"
#include "StringType.h"
#include "SubscriptOperation.h"

namespace Martta
{

MARTTA_OBJECT_CPP(SubscriptOperation);	
	
QString SubscriptOperation::code() const
{
	if (!haveLeft() || !haveRight())
		return "";
	return parenthesise(left()->code() + "[" + right()->code() + "]");
}

bool SubscriptOperation::isValidState() const
{
	// TODO: check subscript index.
	return true;
}

Types SubscriptOperation::allowedTypes(int _index) const
{
	if (_index == 0)
	{
		Types ret;
		foreach (Type t, BareTyped::allowedTypes())
			ret << Type(HashType()).append(t).topWith(Reference()) << Type(HashType()).append(t).topWith(Reference()).topWith(Const())
				<< t.toppedWith(ListType()).topWith(Reference()) << t.toppedWith(ListType()).topWith(Reference()).topWith(Const())
				<< t.toppedWith(AddressType());
			// TODO: All ExplicitTypes in scope with operator[]
		ret << Type(StringType()).topWith(Reference());
		return ret;
	}
	else
	{
		if (leftType()->isType<HashType>())
			return Type(*leftType()->asType<HashType>()->key());
		return Type(Int);
	}
	return Types();
}

Types SubscriptOperation::deniedTypes(int _index) const
{
	if (_index == 0)
	{
		Types ret;
		foreach (Type t, BareTyped::deniedTypes())
			ret << Type(HashType()).append(t) << t.toppedWith(ListType()) << t.toppedWith(AddressType());
			// TODO: All ExplicitTypes in scope with operator[]
		return ret;
	}
	return Types();
}

Type SubscriptOperation::type() const
{
	if (leftType()->isType<StringType>())
		return Type(Wchar).topWith(Reference());
	else if (leftType()->isType<Const>() && leftType()->asType<Const>()->childType()->isType<StringType>())
		return Type(Wchar);
	else if (leftType()->isType<AddressType>())
		return Type(*leftType()->asType<AddressType>()->childType()).topWith(Reference());
	else if (leftType()->isType<Const>() && leftType()->asType<Const>()->childType()->isType<ListType>())
		return Type(*leftType()->asType<Const>()->childType()->asType<ListType>()->childType()).topWith(Const()).topWith(Reference());
	else if (leftType()->isType<ListType>())
		return Type(*leftType()->asType<ListType>()->childType()).topWith(Reference());
	else if (leftType()->isType<Const>() && leftType()->asType<Const>()->childType()->isType<HashType>())
		return Type(*leftType()->asType<Const>()->childType()->asType<HashType>()->value()).topWith(Const()).topWith(Reference());
	else if (leftType()->isType<HashType>())
		return Type(*leftType()->asType<HashType>()->value()).topWith(Reference());
	return Type();
}

bool SubscriptOperation::keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e)
{
	if (_p.exists() && !_p->isPlaceholder() && _p->isKind<Typed>() &&
		(	_p->asKind<Typed>()->type()->isType<ListType>() ||
			_p->asKind<Typed>()->type()->isType<AddressType>() ||
			_p->asKind<Typed>()->type()->isType<HashType>()))
		return simpleKeyPressedOnInsertionPointHandler<SubscriptOperation>(_p, _e, "[", 2, LeftAssociativity);
	else
		return false;
}

}
