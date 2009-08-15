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

#include "TypeEntity.h"
#include "TextLabel.h"
#include "Argument.h"

namespace Martta
{

MARTTA_OBJECT_CPP(Argument);
MARTTA_REGISTER_CSS(Argument,
	L".Argument-Fader { margin-right: -15px; padding-right: 15px; padding-left: 4px; -webkit-border-top-left-radius: 7px; -webkit-border-bottom-left-radius: 7px; background-image: -webkit-gradient(linear, left top, right top, from(#ccc), to(rgba(0, 0, 0, 0))); }"
	L".Argument { text-shadow: 1px 1px 1px #fff; color: #888; font-size: 70%; font-weight: bold; }"
);

String Argument::defineLabelHtml(String const& _text) const
{
	return L"<span class=\"Argument-Fader\"><span class=\"Argument\">_</span></span>" + _text;
}

bool Argument::isSuperfluous() const
{
	return actualType()->isNull();
}

Kinds Argument::allowedKinds(int _i) const
{
	if (_i == Identity)
		return Kind::of<TextLabel>();
	if (_i == OurType)
		return Kind::of<TypeEntity>();
	return Super::allowedKinds(_i);
}

}
