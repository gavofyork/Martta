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

#include "Const.h"
#include "Pointer.h"
#include "ConstLabel.h"
#include "Compound.h"
#include "Reference.h"
#include "Memberify.h"
#include "Argument.h"
#include "MemberLambda.h"

namespace Martta
{

MARTTA_PLACEHOLDER_CPP(MemberLambda);

Kinds MemberLambda::allowedKinds(int _i) const
{
	if (_i == Constness)
		return Kind::of<ConstLabel>();
	if (_i == Body)
		return Kind::of<Compound>();
	if (_i == Returned)
		return Kind::of<TypeEntity>();
	if (_i >= 0)
		return Kind::of<Argument>();
	return Super::allowedKinds(_i);
}

bool MemberLambda::isConst() const
{
	if (ConstLabel* c = tryChild<ConstLabel>(Constness))
		return c->isConst();
	return false;
}

Entity* MemberLambda::isExpander() const
{
	if (body())
		return body()->child(0);
	else
		return 0;
}

/*void MemberLambda::memberDecorate(DecorationContext const& _p) const
{
	if (isConst())
	{
		QRectF r(0.f, 0.f, 16.f, _p.cap(0).height());
		QRgb c = qRgb(0xbb, 0xdd, 0xff);
		QLinearGradient go(QPointF(0.f, 0.f), QPointF(8.f, 0.f));
		go.setColorAt(0.f, qRgba(c, 0));
		go.setColorAt(1.f, qRgba(c, 255));
		_p->setPen(Qt::NoPen);
		_p->setBrush(go);
		_p->drawRect(r);
	}
}*/

String MemberLambda::basicCode(FunctionCodeScope _ref) const
{
	if (!isComplete())
		return String();
	return Martta::code(qualifiers() & MethodMask) + returns()->code(" " + callingCode(_ref)) + (isConst() ? " const" : "");
}

List<Declaration*> MemberLambda::utilised() const
{
	List<Declaration*> l = Super::utilised();
	l.removeAll(ancestor<Class>());
	return l;
}

Type MemberLambda::thisType() const
{
	Type t(classType());
	if (isConst())
		t.topWith(Const());
	return t.topWith(Pointer());
}

String MemberLambda::memberDefineLayout(ViewKeys const& _v) const
{
	return (isConst() ? "M-17;fs11;fb;ewhite;c#5f6f7f;'C';M9;fb0;e#0000;c#0000;fs;" : "") + memberLambdaDefineLayout(_v);
}

bool MemberLambda::keyPressed(KeyEvent const* _e)
{
	AssertNR(isComplete());
	if (LambdaNamer::keyPressed(_e))
		return true;
	else if ((_e->isFocused() || !childIs<Compound>(_e->focalIndex())) && child(Constness) && childAs<Label>(Constness)->keyPressed(_e))
		return true;
	return Super::keyPressed(_e);
}

}
