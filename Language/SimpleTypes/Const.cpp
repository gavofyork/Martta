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
#include "PhysicalType.h"
#include "Reference.h"
#include "Const.h"

namespace Martta
{

MARTTA_OBJECT_CPP(Const);
MARTTA_REGISTER_CSS(Const,
	L".Const-Fader { margin-left: -5px; padding-left: 6px; padding-right: 3px; -webkit-border-top-right-radius: 7px; -webkit-border-bottom-right-radius: 7px; color:#05a; text-shadow: 1px 1px 0px #fff; background-image: -webkit-gradient(linear, right top, left top, from(rgba(0, 127, 255, 0.5)), to(rgba(0, 127, 255, 0.05))); }"
	L".Const { text-shadow: 1px 1px 1px #fff; color:#05a; font-size: 70%; font-weight: bold; }"
	L".ConstLabel-constblock { -webkit-border-image: -webkit-gradient(linear, left top, right top, from(#fff), to(#bdf), color-stop(0.5, #bdf)) 100% 100% 100% 100% repeat repeat; border-width: 0 0 0 12px; padding-left: 8px; margin-left: -20px; }"
	L".ConstLabel-nonconstblock { }"
);

bool Const::defineSimilarityFrom(TypeEntity const* _f, Castability _c) const
{
	return (_f->isKind<Const>() && _f->asKind<Const>()->original()->isSimilarTo(original(), _c))
		|| _f->isSimilarTo(original(), _c)
		|| Super::defineSimilarityFrom(_f, _c);
}

bool Const::defineSimilarityTo(TypeEntity const* _t, Castability _c) const
{
	// Convertible means that a Reference to it will render it Unrelated.
	return (isAnyConvertible(_c) && _c != ConstPerfectConvertible && original()->isSimilarTo(_t, _c)) ||
		(_c == Convertible && original()->isKind<PhysicalType>() && original()->asKind<PhysicalType>()->isCastableTo(_t, true)) ||
		Super::defineSimilarityTo(_t, _c);
}

String Const::code(String const& _middle) const
{
	if (childIs<TypeEntity>(Original))
		return childAs<TypeEntity>(Original)->code(" const" + _middle);
	return String();
}

String Const::defineHtml() const
{
	return toHtml(child(Original)) + L"<span id=\"this\" class=\"Const-Fader\"><span class=\"Const\">CONST</span></span>";
}

Kinds Const::deniedKinds(int _i) const
{
	if (_i == Original)
		return Kind::of<Reference>(), Kind::of<Const>();
	return Super::deniedKinds(_i);
}

}
