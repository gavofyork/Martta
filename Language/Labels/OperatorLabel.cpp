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

#include "OperatorLabel.h"

namespace Martta
{

MARTTA_PROPER_CPP(OperatorLabel);

String OperatorLabel::defineHtml() const
{
	return mark() + tagOf(L"keyword", L"operator") + tagOf(L"symbol", symbolCode());
}

bool OperatorLabel::keyPressed(KeyEvent const* _e)
{
	Operator o = Operator(_e->text() == L")" ? L"()" : _e->text());
	if (!o.isNull())
	{
		m_operator = o;
		changed();
		return true;
	}
	return Super::keyPressed(_e);
}

}
