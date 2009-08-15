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

#include "AccessLabel.h"

namespace Martta
{

MARTTA_OBJECT_CPP(AccessLabel);
MARTTA_REGISTER_CSS(AccessLabel,
	".AccessLabel-publicblock { margin-left: -8px; border-width: 0 0 0 4px; border-color: #efefbf; border-style: solid; padding-left: 4px; }.AccessLabel-protectedblock { margin-left: -8px; border-width: 0 0 0 4px; border-color: #ffdfbf; border-style: solid; padding-left: 4px; }"
	".AccessLabel-privateblock { margin-left: -8px; border-width: 0 0 0 4px; border-color: #ffbfbf; border-style: solid; padding-left: 4px; }"
	".AccessLabel-public { font-weight: bold; text-shadow: -1px -1px 0px #dedebb; }"
	".AccessLabel-protected { font-weight: bold; text-shadow: -1px -1px 0px #ffddbb; }"
	".AccessLabel-private { font-weight: bold; text-shadow: -1px -1px 0px #ffbbbb; }"
);

String AccessLabel::defineHtml() const
{
	return String(L"<span id=\"this\">%1</span>").arg(Martta::code(m_access));
}

bool AccessLabel::keyPressed(KeyEvent const* _e)
{
	if (_e->text() == "U")
		setAccess(Public);
	else if (_e->text() == "O")
		setAccess(Protected);
	else if (_e->text() == "I")
		setAccess(Private);
	else
		return Super::keyPressed(_e);
	return true;
}

};
