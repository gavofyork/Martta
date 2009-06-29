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

#include "Common.h"
#include "AddressType.h"
#include "Reference.h"
#include "Const.h"
#include "Pointer.h"
#include "BasicRoot.h"
#include "ExplicitType.h"
#include "FunctionType.h"
#include "Memberify.h"
#include "LongMemberOperation.h"

namespace Martta
{

MARTTA_OBJECT_CPP(LongMemberOperation);	

Type LongMemberOperation::scope() const
{
	return *leftType()->ignore<Reference>()->ignore<Const>()->ignore<AddressType>();
}
	
Types LongMemberOperation::allowedTypes(int _index) const
{
	if (_index == FirstOperand)
	{
		Types ret;
		foreach (Type i, Super::allowedTypes(FirstOperand))
			ret << i.topWith(Pointer());
		return ret;
	}
	else
		return Super::allowedTypes(_index);
}

QString LongMemberOperation::code() const
{
	if (!haveLeft() || !haveRight()) return "";
	return parenthesise(left()->code() + "->" + right()->code());
}

bool LongMemberOperation::keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e)
{
/*	qDebug() << _p.exists();
	if (_p.exists())
	{
		qDebug() << (!_p->isPlaceholder()) << " " << _p->isKind<Typed>();
		if (_p->isKind<Typed>())
		{	qDebug() << _p->asKind<Typed>()->type()->code();
			_p->debugTree();
			qDebug() << _p->asKind<Typed>()->type()->isType<AddressType>();
			if (_p->asKind<Typed>()->type()->isType<AddressType>())
				qDebug() << _p->asKind<Typed>()->type()->asType<AddressType>()->original()->isType<ExplicitType>();
		}
	}*/
	if (_p.exists() && !_p->isPlaceholder() && _p->isKind<Typed>() &&
		_p->asKind<Typed>()->type()->isType<AddressType>() &&
		_p->asKind<Typed>()->type()->asType<AddressType>()->original()->isType<ExplicitType>())
		return simpleKeyPressedOnInsertionPointHandler<LongMemberOperation>(_p, _e, Operator::XArrow);
	else
		return false;
}

}
