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

MARTTA_PROPER_CPP(Compound);
MARTTA_PROPER_CPP(HardCompound);

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
	else if (_e->text() == L"J" && _e->focalIndex() != UndefinedIndex && _e->focalIndex() >= 0 && _e->focusIsPlaceholder()
		&& child(_e->focalIndex() + 1) && _e->focus()->over().allowedToBeKind(child(_e->focalIndex() + 1)->kind()))
	{
		_e->focus()->replace(child(_e->focalIndex() + 1));
	}
	else if (_e->text() == L";" && _e->focalIndex() != UndefinedIndex)
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

String Compound::statementsToHtml(List<Concept*> const& _es)
{
	String ret;
	bool first = true;
	foreach (Concept const* e, _es)
	{
		if (first || e->isKind<Compound>())
			first = false;
		else
			ret += L"<br/>";
		ret += WebViewable::toHtml(e);
	}
	return ret;
}

String Compound::statementsToHtml(List<Statement*> const& _es)
{
	String ret;
	foreach (Statement const* e, _es)
		ret += WebViewable::toHtml(e, e->requiresSemicolon() ? L"span" : L"div") + (e->requiresSemicolon() ? L"<br/>" : L"");
	return ret;
}

String Compound::defineHtml() const
{
	if (statements().size() > 1)
		return L"<div class=\"minor symbol\">{</div><div class=\"block\">" + statementsToHtml(statements()) + L"</div><div class=\"minor symbol\">}</div>";
	else if (statements().size())
		return L"<div class=\"block\">" + statementsToHtml(statements()) + L"</div>";
	else
		return L" <span class=\"minor symbol\">{}</span>";
}

String HardCompound::defineHtml() const
{
	return L"<div class=\"minor symbol\">{</div><div class=\"block\">" + statementsToHtml(statements()) + L"</div><div class=\"minor symbol\">}</div>";
}

}
