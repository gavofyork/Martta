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

#include "IdentifierSet.h"
#include "Compound.h"
#include "BreakStatement.h"
#include "Loop.h"

namespace Martta
{

MARTTA_PROPER_CPP(Loop);

static SimpleIdentifierSet<Loop> s_loopSet(L"forever");

bool Loop::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	return simplePlaceholderKeyPressHandler<Loop>(_p, _e, "...");
}

Kinds Loop::allowedKinds(int _index) const
{
	if (_index == Body)
		return Kind::of<Compound>();
	return Super::allowedKinds(_index);
}

bool Loop::keyPressed(KeyEvent const* _e)
{
	if (Corporal::keyPressed(_e))
	{}
	else if (_e->text() == "B" && _e->focus()->isPlaceholder() && _e->focus()->over().allowedToBeKind<BreakStatement>())
	{
		Concept* e = new BreakStatement;
		_e->focus()->replace(e);
		e->setCurrent();
	}
	else
		return Super::keyPressed(_e);
	return true;
}

String Loop::defineHtml() const
{
	return L"<^><span class=\"keyword\">forever</span><br/>" + Corporal::defineHtml(true);
}

String Loop::code() const
{
	return String(L"while (true)\n") + (asStatement(Body) ? asStatement(Body)->codeAsStatement() : "");
}

}
