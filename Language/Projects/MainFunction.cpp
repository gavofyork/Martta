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

#include "BuiltinType.h"
#include "Argument.h"
#include "TextLabel.h"

#include "MainFunction.h"

namespace Martta
{

MARTTA_OBJECT_CPP(MainFunction);

bool MainFunction::keyPressed(KeyEvent const* _e)
{
	return Super::keyPressed(_e);
}

Kinds MainFunction::allowedKinds(int _i) const
{
	return Super::allowedKinds(_i);
}

String MainFunction::defineMidLayout(ViewKeys&) const
{
	return L"'[ENTRY]'";
}

String MainFunction::definePreHtml() const
{
	return L"<span style=\"background-color: #4c4; -webkit-border-radius: 2px\" class=\"badge\">&crarr;</span> " + Super::definePreHtml();
}

}
