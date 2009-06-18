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
	
bool IfStatement::keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e)
{
	return simplePlaceholderKeyPressHandler<IfStatement>(_p, _e, "?");
}
	
QString IfStatement::code() const
{
	QString ret;
	ret += "if (" + (isStatement(0) ? asStatement(0)->code() : "");
	ret += ")\n" + (isStatement(1) ? asStatement(1)->codeAsStatement() : "");
	ret += "else\n" + (isStatement(2) ? asStatement(2)->codeAsStatement() : "{}");
	return ret;
}

Kinds IfStatement::allowedKinds(int _index) const
{
	switch (_index)
	{
		case 0: return Kind::of<BareTyped>();
		case 1:
		case 2: return Kind::of<Compound>();
		default: return Super::allowedKinds(_index);
	}
}

Types IfStatement::allowedTypes(int _index) const
{
	switch (_index)
	{
		case 0: return Type(Bool).topWith(Const());
		default: return Types();
	}
}

bool IfStatement::keyPressed(EntityKeyEvent const* _e)
{
	if ((_e->text() == ")" || _e->text() == "{") && _e->focalIndex() == 0 && childrenOf<Compound>().size())
		childrenOf<Compound>()[0]->navigateOnto(_e->codeScene());
	else if (_e->text() == "E" && _e->focalIndex() != 2)
	{
		if (!child(2)) back().place(new Compound);
		child(2)->navigateOnto(_e->codeScene());
	}
	else
		return Super::keyPressed(_e);
	return true;
}

QString IfStatement::defineLayout(ViewKeys&) const
{
	return QString("ycode;^;'if (';0;')'") +
			(child(1) && child(1)->cardinalChildCount() ? ";n;i;1" : ";1") +
			(child(2) ? ";n;'else'" + QString(child(2)->cardinalChildCount() ? ";n;i;2" : ";2") : QString());
}

}
