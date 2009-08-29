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

#include "ConstLabel.h"

namespace Martta
{

MARTTA_OBJECT_CPP(ConstLabel);

String ConstLabel::defineHtml() const
{
	return String(L"<^><span class=\"keyword\">%1</span>").arg(m_isConst ? L"const" : L"mutable");
}

bool ConstLabel::keyPressed(KeyEvent const* _e)
{
	if (_e->text() == "!" || _e->text() == L"\t")
		setIsConst(!isConst());
	else
		return Super::keyPressed(_e);
	return true;
}

}
