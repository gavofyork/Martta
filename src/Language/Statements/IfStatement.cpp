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

#include "BareTyped.h"
#include "Const.h"
#include "Compound.h"
#include "IfStatement.h"

namespace Martta
{

MARTTA_OBJECT_CPP(IfStatement);	
	
bool IfStatement::keyPressedOnPosition(Position const& _p, EntityKeyEvent const* _e)
{
	return simplePlaceholderKeyPressHandler<IfStatement>(_p, _e, "?");
}
	
String IfStatement::code() const
{
	String ret;
	ret += "if (" + (isStatement(Condition) ? asStatement(Condition)->code() : "");
	ret += ")\n" + (isStatement(Body) ? asStatement(Body)->codeAsStatement() : "");
	ret += "else\n" + (isStatement(AltBody) ? asStatement(AltBody)->codeAsStatement() : "{}");
	return ret;
}

Kinds IfStatement::allowedKinds(int _index) const
{
	if (_index == Condition)
		return Kind::of<BareTyped>();
	if (_index == Body || _index == AltBody)
		return Kind::of<Compound>();
	return Super::allowedKinds(_index);
}

Types IfStatement::allowedTypes(int _index) const
{
	if (_index == Condition)
		return Type(Bool).topWith(Const());
	return Super::allowedTypes(_index);
}

bool IfStatement::keyPressed(EntityKeyEvent const* _e)
{
	if (Corporal::keyPressed(_e))
	{}
	else if (_e->text() == "E" && _e->focalIndex() != AltBody)
	{
		if (!child(AltBody))
			back().place(new Compound);
		child(AltBody)->navigateOnto(_e->codeScene());
	}
	else
		return Super::keyPressed(_e);
	return true;
}

String IfStatement::defineLayout(ViewKeys const& _k) const
{
	return String("ycode;^;'if (';%1;')'").arg(Condition) + Corporal::defineLayout(_k, true) +
			(child(AltBody) ? ";n;'else'" + String(child(AltBody)->cardinalChildCount() ? ";n;i;%1" : ";%1").arg(AltBody) : String());
}

}
