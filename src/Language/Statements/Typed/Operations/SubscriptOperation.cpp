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
	if (_index == FirstOperand)
	{
		Types ret;
		foreach (Type t, BareTyped::allowedTypes())
			ret << t.toppedWith(HashType()).topWith(Reference()) << t.toppedWith(HashType()).topWith(Reference()).topWith(Const())
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
	if (_index == FirstOperand)
	{
		Types ret;
		foreach (Type t, BareTyped::deniedTypes())
			ret << t.toppedWith(HashType()) << t.toppedWith(ListType()) << t.toppedWith(AddressType());
			// TODO: All ExplicitTypes in scope with operator[]
		return ret;
	}
	return Types();
}

Type SubscriptOperation::type() const
{
	if (leftType()->isType<StringType>())
		return Type(Wchar).topWith(Reference());
	else if (leftType()->isType<Const>() && leftType()->asType<Const>()->original()->isType<StringType>())
		return Type(Wchar);
	else if (leftType()->isType<AddressType>())
		return Type(*leftType()->asType<AddressType>()->original()).topWith(Reference());
	else if (leftType()->isType<Const>() && leftType()->asType<Const>()->original()->isType<ListType>())
		return Type(*leftType()->asType<Const>()->original()->asType<ListType>()->original()).topWith(Const()).topWith(Reference());
	else if (leftType()->isType<ListType>())
		return Type(*leftType()->asType<ListType>()->original()).topWith(Reference());
	else if (leftType()->isType<Const>() && leftType()->asType<Const>()->original()->isType<HashType>())
		return Type(*leftType()->asType<Const>()->original()->asType<HashType>()->value()).topWith(Const()).topWith(Reference());
	else if (leftType()->isType<HashType>())
		return Type(*leftType()->asType<HashType>()->value()).topWith(Reference());
	return Type();
}

bool SubscriptOperation::keyPressed(EntityKeyEvent const* _e)
{
	if (_e->text() == "]")
		setCurrent();
	else
		return Super::keyPressed(_e);
	return true;
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
