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

#include "CodeScene.h"
#include "ValueDefiner.h"
#include "SpecialKeys.h"
#include "Compound.h"

namespace Martta
{

MARTTA_OBJECT_CPP(Compound);
MARTTA_OBJECT_CPP(HardCompound);

String Compound::code() const
{
	String ret = "{\n";
	foreach (Statement* s, statements())
		ret += s->codeAsStatement() + "\n";
	ret += "\n}";
	return ret;
}

void Compound::appendDefinedUptoHere(int _i, List<ValueDefiner*>* _list) const
{
	for(int i = 0; i < _i; ++i)
		if (ValueDefiner* v = tryChild<ValueDefiner>(i))
			*_list += v;
}

bool Compound::keyPressed(KeyEvent const* _e)
{
	if (_e->text() == ReturnKey)
	{
		Position p = (_e->isFocused()) ?
		(_e->isInserting() || _e->modifiers() & ShiftModifier) ?
		front() :
		back() :
		middle(_e->focalIndex() + ((_e->isInserting() || _e->modifiers() & ShiftModifier) ? 0 : 1));
		Statement* s = new Statement;
		p.place(s);
		s->setCurrent();
	}
	else if (_e->text() == L"¬" && _e->focalIndex() != UndefinedIndex)
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

String Compound::defineLayout(ViewKeys const&) const
{
	if (statements().size() > 1)
		return "ycode;-i;'{';n;" + times(0, cardinalChildCount(), ";n;") + ";n;-i;'}'";
	else if (statements().size())
		return "ycode;0";
	else
		return "ycode;'{}'";
}

String Compound::defineHtml() const
{
	if (statements().size() > 1)
		return L"<div class=\"deblock minor symbol\">{</div>" + toHtml(cardinalChildren(), L"<br/>") + L"<div class=\"deblock minor symbol\">}</div>";
	else if (statements().size())
		return toHtml(statements()[0]);
	else
		return L"<span class=\"minor symbol\">{}</span>";
}

String HardCompound::defineLayout(ViewKeys const&) const
{
	return "ycode;-i;'{';n;" + times(0, cardinalChildCount(), ";n;") + ";n;-i;'}'";
}

String HardCompound::defineHtml() const
{
	return L"<span class=\"minor symbol\">{</span><br/><div class=\"block\">" + toHtml(cardinalChildren(), L"<br/>") + L"</div><span class=\"minor symbol\">}</div>";
}

}
