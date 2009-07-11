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

#include "CompletionDelegate.h"
#include "GlobalReferenced.h"

namespace Martta
{

MARTTA_OBJECT_CPP(GlobalReferenced);

bool GlobalReferenced::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	return simplePositionKeyPressHandler<GlobalReferenced>(_p, _e, L":");
}

bool GlobalReferenced::keyPressed(KeyEvent const* _e)
{
	if (_e->text().length() == 1 && _e->text()[0].isLetter())
	{
		setEditing(_e->codeScene());
		_e->reinterpretLater();
	}
	else
		return Super::keyPressed(_e);
	return true;
}

String GlobalReferenced::defineLayout(ViewKeys const& _k) const
{
	return L"p:/global.svg;" + Super::defineLayout(_k);
}

String GlobalReferenced::defineEditLayout(ViewKeys const& _k, ValueDefiner* _v)
{
	return L"p:/global.svg;" + Super::defineEditLayout(_k, _v);
}

List<ValueDefiner*> GlobalReferenced::possibilities(Position const& _p)
{
	return _p->root()->childrenOf<ValueDefiner>();
}

}
