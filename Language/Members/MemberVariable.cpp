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

#include "WebStylistRegistrar.h"
#include "TextLabel.h"
#include "TypeEntity.h"
#include "Reference.h"
#include "MemberVariable.h"

namespace Martta
{

MARTTA_OBJECT_CPP(MemberVariable);
MARTTA_REGISTER_CSS(MemberVariable,
	L".MemberVariable-Fader { margin-right: -15px; padding-right: 15px; padding-left: 4px; color:#444; text-shadow: 1px 1px 0px #fff; -webkit-border-top-left-radius: 7px; -webkit-border-bottom-left-radius: 7px; background-image: -webkit-gradient(linear, left top, right top, from(#999), to(rgba(0, 0, 0, 0))); }"
	L".MemberVariable { text-shadow: 1px 1px 1px #fff; color:#555; font-size: 70%; font-weight: bold; }"
);

String MemberVariable::defineLabelHtml(String const& _text) const
{
	return L"<span class=\"MemberVariable-Fader\"><span class=\"MemberVariable\">M</span></span>" + _text;
}

Kinds MemberVariable::allowedKinds(int _i) const
{
	if (_i == OurType)
		return Kind::of<TypeEntity>();
	return Super::allowedKinds(_i);
}

bool MemberVariable::keyPressedOnPosition(Position const& _p, KeyEvent const* _e)
{
	return simplePositionKeyPressHandler<MemberVariable>(_p, _e, L"F");
}

}
