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

#include "Position.h"
#include "CompletionDelegate.h"
#include "GlobalReferencedValue.h"

namespace Martta
{

MARTTA_PROPER_CPP(GlobalReferencedValue);
MARTTA_REGISTER_CSS(GlobalReferencedValue,
//	L".GlobalReferencedValue-Fader { margin-right: -15px; padding-right: 15px; padding-left: 4px; -webkit-border-top-left-radius: 7px; -webkit-border-bottom-left-radius: 7px; background-image: -webkit-gradient(linear, left top, right top, from(#ccc), to(rgba(0, 0, 0, 0))); }"
	L".GlobalReferencedValue { text-shadow: 1px 1px 1px #fff; color: #888; font-size: 70%; font-weight: bold; }"
);

bool GlobalReferencedValue::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	return simplePositionKeyPressHandler<GlobalReferencedValue>(_p, _e, L":");
}

bool GlobalReferencedValue::keyPressed(KeyEvent const* _e)
{
	if (_e->text().length() == 1 && _e->text()[0].isLetter())
	{
		_e->codeScene()->setEditing(this);
		if (_e->codeScene()->isEditing(this))
			_e->reinterpretLater();
	}
	else
		return Super::keyPressed(_e);
	return true;
}

String GlobalReferencedValue::defineHtml() const
{
	return L"<span class=\"GlobalReferencedValue-Fader\"><span class=\"GlobalReferencedValue\">::</span></span>" + Super::defineHtml();
}

String GlobalReferencedValue::defineEditHtml(CodeScene* _cs) const
{
	return L"<span class=\"GlobalReferencedValue-Fader\"><span class=\"GlobalReferencedValue\">::</span></span>" + Super::defineEditHtml(_cs);
}

List<ValueDefiner*> GlobalReferencedValue::staticRefPossibilities(Position const& _p)
{
	return _p->root()->childrenOf<ValueDefiner>();
}

}
