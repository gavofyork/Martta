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

#include "Type.h"
#include "BuiltinType.h"
#include "Subscriptable.h"
#include "SubscriptableRegistrar.h"
#include "SubscriptOperation.h"

namespace Martta
{

MARTTA_PROPER_CPP(SubscriptOperation);	
	
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
	}
	return Super::allowedTypes(_index);
}

Type SubscriptOperation::type() const
{
	Type lt = leftType();
	if (Subscriptable* s = lt->tryType<Subscriptable>())
		return s->subscriptsTo(rightType());
	return Type();
}

bool SubscriptOperation::keyPressed(KeyEvent const* _e)
{
	if (_e->text() == "]")
		_e->codeScene()->setCurrent(this);
	else
		return Super::keyPressed(_e);
	return true;
}

bool SubscriptOperation::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	if (_p.exists() && !_p->isPlaceholder()/* && _p->isKind<Typed>() && SubscriptableRegistrar::get()->m_acceptable.values().contains(_p->asKind<Typed>()->apparentType())*/)
		return simpleKeyPressedOnPositionHandler<SubscriptOperation>(_p, _e, "[", 2, LeftAssociativity);
	else
		return false;
}

}
