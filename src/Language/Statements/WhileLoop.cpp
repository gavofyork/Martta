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
	ret += "while (" + (asStatement(0) ? asStatement(0)->code() : "");
	ret += ")\n" + (asStatement(1) ? asStatement(1)->codeAsStatement() : "");
	return ret;
}

Kinds WhileLoop::allowedKinds(int _index) const
{
	switch (_index)
	{
		case 0: return Kind::of<BareTyped>();
		case 1: return Kind::of<Compound>();
		default: return Super::allowedKinds(_index);
	}
}

Types WhileLoop::allowedTypes(int _index) const
{
	switch (_index)
	{
		case 0: return Type(Bool).topWith(Const());
		default: return Types();
	}
}

QString WhileLoop::defineLayout(ViewKeys&) const
{
	return QString("ycode;^;'while (';0;')'") +
		(entity(1) && entity(1)->entities().size() ? ";n;i;1" : ";1");
}

bool WhileLoop::keyPressed(EntityKeyEvent const* _e)
{
	if ((_e->text() == ")" || _e->text() == "{") && _e->focalIndex() == 0 && allEntitiesOf<Compound>().size())
		allEntitiesOf<Compound>()[0]->navigateOnto(_e->codeScene());
	else
		return Super::keyPressed(_e);
	return true;
}

}
