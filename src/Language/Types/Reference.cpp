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
#include "Const.h"
#include "Type.h"
#include "Reference.h"

namespace Martta
{

MARTTA_OBJECT_CPP(Reference);

bool Reference::defineSimilarityTo(TypeEntity const* _t, Castability _c) const
{
			// X -*> Y => X& -=> Y&
	return _t->isKind<Reference>() && childType()->isSimilarTo(_t->asKind<Reference>()->childType(), Physical) ||
			// X -O3> Y => X& -O3> Y
			_c == VeryConvertible && childType()->isSimilarTo(_t, VeryConvertible) ||
			// X -O2> Y => X& -O2> Y
			_c == FairlyConvertible && childType()->isSimilarTo(_t, FairlyConvertible) ||
			// X -O1> Y => X& -O1> Y
			_c == Convertible && childType()->isSimilarTo(_t, Convertible) ||
		Super::defineSimilarityTo(_t, _c);
}

bool Reference::defineSimilarityFrom(TypeEntity const* _f, Castability _c) const
{
	return isAnyConvertible(_c) && childType()->isKind<Const>() && 
			// X -O2> X const&
			(_f->isEquivalentTo(childType()->asKind<Const>()->childType()) || _f->isSimilarTo(childType()->asKind<Const>()->childType(), _c) ||
			// X const -O2> X const&
			_f->isKind<Const>() && (_f->asKind<Const>()->childType()->isSimilarTo(childType()->asKind<Const>()->childType(), _c) ||
			_f->asKind<Const>()->childType()->isEquivalentTo(childType()->asKind<Const>()->childType()) ) ) ||
		Super::defineSimilarityFrom(_f, _c);
}

QString Reference::defineLayout(ViewKeys&) const
{
	return "0;(;M1;^;fb;s;e#ffffff;c#7f6f5f;fs-2;'&';M2;)";
}

void Reference::decorate(DecorationContext const& _c) const
{
	QRectF r = alignedForUnitPen(_c(1));
	r.setX(r.x() - 16);

	QRgb c = qRgb(0xff, 0xdd, 0xbb);
	
	QRadialGradient go(_c(1).center(), 24);
	go.setColorAt(0.f, qRgba(c, 255));
	go.setColorAt(1.f, qRgba(c, 0));

	QLinearGradient gp(_c(1).topRight() - QPointF(8.f, 0.f), _c(1).topLeft());
	gp.setColorAt(0.f, qRgba(c, 255));
	gp.setColorAt(1.f, qRgba(c, 0));
	_c->setPen(Qt::NoPen);
	_c->setBrush(go);
	_c->drawRoundRect(r, 50, 100);
}

}
