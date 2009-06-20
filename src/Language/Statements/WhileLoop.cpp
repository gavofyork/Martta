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

#include <QtXml>

#include "BareTyped.h"
#include "Const.h"
#include "Compound.h"
#include "WhileLoop.h"

namespace Martta
{

MARTTA_OBJECT_CPP(WhileLoop);	
	
QString WhileLoop::code() const
{
	QString ret;
	ret += "while (" + (asStatement(Condition) ? asStatement(Condition)->code() : "");
	ret += ")\n" + (asStatement(Body) ? asStatement(Body)->codeAsStatement() : "");
	return ret;
}

Kinds WhileLoop::allowedKinds(int _index) const
{
	if (_index == Condition)
		return Kind::of<BareTyped>();
	if (_index == Body)
		return Kind::of<Compound>();
	return Super::allowedKinds(_index);
}

Types WhileLoop::allowedTypes(int _index) const
{
	if (_index == Condition)
		return Type(Bool).topWith(Const());
	return Types();
}

QString WhileLoop::defineLayout(ViewKeys& _k) const
{
	return ("ycode;^;'while (';%1;')'" + Corporal::defineLayout(_k, true)).arg(Condition);
}

bool WhileLoop::keyPressed(EntityKeyEvent const* _e)
{
	if (Corporal::keyPressed(_e))
		return true;
	else
		return Super::keyPressed(_e);
	return true;
}

}
