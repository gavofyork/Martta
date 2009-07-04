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

#include "CommonGraphics.h"
#include "DecorationContext.h"
#include "Memberify.h"
#include "AddressType.h"
#include "ExplicitType.h"
#include "SimpleType.h"
#include "FunctionType.h"
#include "Reference.h"
#include "Const.h"

namespace Martta
{

MARTTA_OBJECT_CPP(Const);

bool Const::defineSimilarityFrom(TypeEntity const* _f, Castability _c) const
{
	return _f->isKind<Const>() && _f->asKind<Const>()->original()->isSimilarTo(original(), _c) 
		|| _f->isSimilarTo(original(), _c)
		|| Super::defineSimilarityFrom(_f, _c);
}

bool Const::defineSimilarityTo(TypeEntity const* _t, Castability _c) const
{
	// Convertible means that a Reference to it will render it Unrelated.
	return isAnyConvertible(_c) && _c != ConstPerfectConvertible && original()->isSimilarTo(_t, _c) || 
		_c == Convertible && original()->isKind<ExplicitType>() && original()->asKind<ExplicitType>()->haveSingleCastOperator(_t, true) ||
		Super::defineSimilarityTo(_t, _c);
}

String Const::code(String const& _middle) const
{
	if (childIs<TypeEntity>(Original))
		return childAs<TypeEntity>(Original)->code(" const" + _middle);
	return String();
}

String Const::defineLayout(ViewKeys const&) const
{
	return String("%1;(;M2;^;fb;s;ewhite;c#5f6f7f;fs-2;'C';M3;)").arg(Original);
}

/*void Const::decorate(DecorationContext const& _c) const
{
	QRectF r = alignedForUnitPen(_c(1));
	r.setX(r.x() - 16);
	
	QRgb c = qRgb(0xbb, 0xdd, 0xff);
	
	QRadialGradient go(_c(1).center(), 24);
	go.setColorAt(0.f, qRgba(c, 255));
	go.setColorAt(1.f, qRgba(c, 0));

	_c->setPen(Qt::NoPen);
	_c->setBrush(go);
	_c->drawRoundRect(r, 50, 100);
}*/

Kinds Const::deniedKinds(int _i) const
{
	if (_i == Original)
		return Kind::of<Reference>(), Kind::of<Const>();
	return Super::deniedKinds(_i);
}

}
