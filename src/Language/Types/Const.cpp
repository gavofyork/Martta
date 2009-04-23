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
	return _f->isKind<Const>() && _f->asKind<Const>()->child()->isSimilarTo(child(), _c) 
		|| _f->isSimilarTo(child(), _c)
		|| Super::defineSimilarityFrom(_f, _c);
}

bool Const::defineSimilarityTo(TypeEntity const* _t, Castability _c) const
{
	// Convertible means that a Reference to it will render it Unrelated.
	return isAnyConvertible(_c) && _c != ConstPerfectConvertible && child()->isSimilarTo(_t, _c) || 
		_c == Convertible && child()->isKind<ExplicitType>() && child()->asKind<ExplicitType>()->haveSingleCastOperator(_t, true) ||
		Super::defineSimilarityTo(_t, _c);
}

QString Const::code(QString const& _middle) const
{
	if (entityIs<TypeEntity>(0))
		return entityAs<TypeEntity>(0)->code(" const" + _middle);
	return QString();
}

QString Const::defineLayout(ViewKeys&) const
{
	return "0;(;M2;^;fb;s;ewhite;c#5f6f7f;fs-2;'C';M3;)";
}

void Const::decorate(DecorationContext const& _c) const
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
}

Kinds Const::deniedKinds(int _i) const
{
	if (_i == 0)
		return Kind::of<Reference>(), Kind::of<Const>();
	return Super::deniedKinds(_i);
}

Kinds Const::allowedKinds(int _i) const
{
	if (_i == 0)
		return Kind::of<TypeEntity>();
	return Kinds();
}

}
