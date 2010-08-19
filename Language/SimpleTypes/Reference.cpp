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
#include "Const.h"
#include "Type.h"
#include "Reference.h"

namespace Martta
{

MARTTA_PROPER_CPP(Reference);
MARTTA_REGISTER_CSS(Reference,
	L".Reference-Fader { margin-left: -5px; padding-left: 6px; padding-right: 3px; -webkit-border-top-right-radius: 7px; -webkit-border-bottom-right-radius: 7px; text-shadow: 1px 1px 0px #fff; background-image: -webkit-gradient(linear, right top, left top, from(rgba(255, 127, 0, 0.5)), to(rgba(255, 127, 0, 0.05))); }"
	L".Reference { text-shadow: 1px 1px 1px #fff; color:#a50; font-size: 70%; font-weight: bold; }"
);

Type Reference::ensure(Type const& _t)
{
	if (!_t->isType<Reference>())
		return _t.toppedWith(Reference());
	return _t;
}

bool Reference::defineSimilarityTo(TypeConcept const* _t, Castability _c) const
{
			// X -*> Y => X& -=> Y&
	return (_t->isKind<Reference>() && original()->isSimilarTo(_t->asKind<Reference>()->original(), Physical)) ||
			// X -O3> Y => X& -O3> Y
			(_c == VeryConvertible && original()->isSimilarTo(_t, VeryConvertible)) ||
			// X -O2> Y => X& -O2> Y
			(_c == FairlyConvertible && original()->isSimilarTo(_t, FairlyConvertible)) ||
			// X -O1> Y => X& -O1> Y
			(_c == Convertible && original()->isSimilarTo(_t, Convertible)) ||
		Super::defineSimilarityTo(_t, _c);
}

bool Reference::defineSimilarityFrom(TypeConcept const* _f, Castability _c) const
{
	return (isAnyConvertible(_c) && original()->isKind<Const>() &&
			// X -O2> X const&
			(_f->isEquivalentTo(original()->asKind<Const>()->original()) || _f->isSimilarTo(original()->asKind<Const>()->original(), _c) ||
			// X const -O2> X const&
			(_f->isKind<Const>() && (_f->asKind<Const>()->original()->isSimilarTo(original()->asKind<Const>()->original(), _c) ||
			_f->asKind<Const>()->original()->isEquivalentTo(original()->asKind<Const>()->original()) ) ) )) ||
		Super::defineSimilarityFrom(_f, _c);
}

String Reference::defineHtml() const
{
	return toHtml(child(Original)) + L"<^><span class=\"Reference-Fader\"><span class=\"Reference\">&</span></span>";
}

}
