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

#include "LongMemberOperation.h"
#include "Reference.h"
#include "ExplicitType.h"
#include "Const.h"
#include "RootEntity.h"
#include "FunctionType.h"
#include "AddressType.h"
#include "Memberify.h"
#include "MemberOperation.h"

namespace Martta
{

MARTTA_OBJECT_CPP(MemberOperation);	
	
Types MemberOperation::allowedTypes(int _index) const
{
	if (_index == 0)
	{
		Types ret;
		foreach (Type i, Super::allowedTypes(0))
			ret << i.topWith(Reference());
		return ret;
	}
	else
		return Super::allowedTypes(_index);
}

Type MemberOperation::scope() const
{
	return *leftType()->ignore<Reference>();
}
	
QString MemberOperation::code() const
{
	if (!haveLeft() || !haveRight()) return "";
	return parenthesise(left()->code() + "." + right()->code());
}

bool MemberOperation::keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e)
{
	if (_p.exists() && !_p->isPlaceholder() && _p->isKind<Typed>() &&
		_p->asKind<Typed>()->type()->isType<Reference>())
		return simpleKeyPressedOnInsertionPointHandler<MemberOperation>(_p, _e, ".", 2, LeftAssociativity);
	else
		return false;
}

}
