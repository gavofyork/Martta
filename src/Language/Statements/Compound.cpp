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
#include "CodeScene.h"
#include "LambdaNamer.h"	// For determining look of compound.
#include "Compound.h"

namespace Martta
{

MARTTA_OBJECT_CPP(Compound);	
	
String Compound::code() const
{
	String ret = "{\n";
	foreach (Statement* s, statements())
		ret += s->codeAsStatement() + "\n";
	ret += "\n}";
	return ret;
}

void Compound::appendDefinedUptoHere(int _i, QList<ValueDefiner*>* _list) const
{
	if (_i >= 0)
		for(int i = 0; i < _i; ++i)
			if (childIs<ValueDefiner>(i))
				*_list += childAs<ValueDefiner>(i);
}

String Compound::defineLayout(ViewKeys const&) const
{
	if (statements().size() > 1 || parentIs<LambdaNamer>())
		return "ycode;-i;'{';n;" + times(0, cardinalChildCount(), ";n;") + ";n;-i;'}'";
	else if (statements().size())
		return "ycode;0";
	else
		return "ycode;'{}'";
}

bool Compound::keyPressed(EntityKeyEvent const* _e)
{
	if (_e->key() == Qt::Key_Return)
	{
		Position p = (_e->isFocused()) ?
		(/*_e->inserting() || */_e->modifiers() & Qt::ShiftModifier) ?
		front() :
		back() :
		middle(_e->focalIndex() + ((/*_e->inserting() || */_e->modifiers() & Qt::ShiftModifier) ? 0 : 1));
		Statement* s = new Statement;
		p.place(s);
		s->setCurrent();
	}
	else if (_e->key() == Qt::Key_Home && _e->focalIndex() != UndefinedIndex)
	{
		child(_e->focalIndex())->setCurrent();
	}
	else if (_e->text() == "{")
	{
		child(0)->setCurrent();
	}
	else
		return Super::keyPressed(_e);
	return true;
}

}
