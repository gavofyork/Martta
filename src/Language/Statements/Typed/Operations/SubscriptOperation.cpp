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
#include "Subscriptable.h"
#include "SubscriptableRegistrar.h"
#include "SubscriptOperation.h"

namespace Martta
{

MARTTA_OBJECT_CPP(SubscriptOperation);	
	
String SubscriptOperation::code() const
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
		return SubscriptableRegistrar::get()->m_acceptable.values();
/*		Types ret;
		foreach (Type t, BareTyped::ourAllowedTypes())
			ret << t.toppedWith(HashType()) //.topWith(Reference()) << t.toppedWith(HashType()).topWith(Reference()).topWith(Const())
				<< t.toppedWith(ListType()) //.topWith(Reference()) << t.toppedWith(ListType()).topWith(Reference()).topWith(Const())
				<< t.toppedWith(AddressType());
			// TODO: All ExplicitTypes in scope with operator[]
		ret << Type(StringType());//.topWith(Reference());
		return ret;*/
	}
	else if (_index == SecondOperand)
	{
		Type lt = leftType();
		if (Subscriptable* s = lt->tryType<Subscriptable>())
		{
			Types ts = s->subscriptTypes();
			ts.replace(Type(), Type(Unsigned|Int));
			return ts;
		}
		return Types();
/*		if (leftType()->isType<HashType>())
			return Type(*leftType()->asType<HashType>()->key());
		return Type(Int);
*/	}
	return Super::allowedTypes(_index);
}

/*Types SubscriptOperation::deniedTypes(int _index) const
{
	if (_index == FirstOperand)
	{
		Types ret;
		foreach (Type t, BareTyped::ourDeniedTypes())
			ret << t.toppedWith(HashType()) << t.toppedWith(ListType()) << t.toppedWith(AddressType());
			// TODO: All ExplicitTypes in scope with operator[]
		return ret;
	}
	else if (_index == SecondOperand)
	{
		return Types();
	}
	return Super::deniedTypes(_index);
}*/

Type SubscriptOperation::type() const
{
	Type lt = leftType();
	if (Subscriptable* s = lt->tryType<Subscriptable>())
		return s->subscriptsTo(rightType());
	return Type();
	/*
	if (leftType()->isType<StringType>())
		return Type(Wchar).topWith(Reference());
	else if (leftType()->isType<Const>() && leftType()->asType<Const>()->original()->isType<StringType>())
		return Type(Wchar);
	else if (leftType()->isType<Const>() && leftType()->asType<Const>()->original()->isType<ListType>())
		return Type(*leftType()->asType<Const>()->original()->asType<ListType>()->original()).topWith(Const()).topWith(Reference());
	else if (leftType()->isType<ListType>())
		return Type(*leftType()->asType<ListType>()->original()).topWith(Reference());
	else if (leftType()->isType<Const>() && leftType()->asType<Const>()->original()->isType<HashType>())
		return Type(*leftType()->asType<Const>()->original()->asType<HashType>()->value()).topWith(Const()).topWith(Reference());
	else if (leftType()->isType<HashType>())
		return Type(*leftType()->asType<HashType>()->value()).topWith(Reference());
	return Type();*/
}

bool SubscriptOperation::keyPressed(KeyEvent const* _e)
{
	if (_e->text() == "]")
		setCurrent();
	else
		return Super::keyPressed(_e);
	return true;
}

bool SubscriptOperation::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	if (_p.exists() && !_p->isPlaceholder()/* && _p->isKind<Typed>() && SubscriptableRegistrar::get()->m_acceptable.values().contains(_p->asKind<Typed>()->type())*/)
/*		(	_p->asKind<Typed>()->type()->isType<ListType>() ||
			_p->asKind<Typed>()->type()->isType<AddressType>() ||
			_p->asKind<Typed>()->type()->isType<HashType>()))*/
		return simpleKeyPressedOnPositionHandler<SubscriptOperation>(_p, _e, "[", 2, LeftAssociativity);
	else
		return false;
}

}
