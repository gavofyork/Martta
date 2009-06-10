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
#include "ConstLabel.h"
#include "Compound.h"
#include "Reference.h"
#include "Memberify.h"
#include "MemberLambda.h"

namespace Martta
{

MARTTA_PLACEHOLDER_CPP(MemberLambda);

Type MemberLambda::type() const
{
	if (!isComplete())
		return Type();
	Type ret = LambdaNamer::type();
	if (!ancestor<Class>() || !ret->isType<Reference>() || !ret->asType<Reference>()->child())
		return Type();
	ret->asType<Reference>()->child()->knitIn(new Memberify(ancestor<Class>(), isConst()));
	return ret;
}

bool MemberLambda::isConst() const
{
	if (!isComplete())
		return false;
	QList<ConstLabel*> l = entitiesOf<ConstLabel>();
	return l.size() ? l[0]->isConst() : false;
}

Entity* MemberLambda::isExpander() const
{
	return body()->entity(0);
}

void MemberLambda::memberDecorate(DecorationContext const& _p) const
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
}

QString MemberLambda::basicCode(FunctionCodeScope _ref) const
{
	if (!isComplete())
		return QString();
	return LambdaNamer::basicCode(_ref) + (isConst() ? " const" : "");
}

QList<DeclarationEntity*> MemberLambda::utilised() const
{
	QList<DeclarationEntity*> l = Super::utilised();
	l.removeAll(ancestor<Class>());
	return l;
}

QString MemberLambda::memberDefineLayout(ViewKeys& _v) const
{
	return (isConst() ? "M-17;fs11;fb;ewhite;c#5f6f7f;'C';M9;fb0;e#0000;c#0000;fs;" : "") + LambdaNamer::defineLayout(_v);
}

bool MemberLambda::keyPressed(EntityKeyEvent const* _e)
{
	M_ASSERT(isComplete());
	if (LambdaNamer::keyPressed(_e))
		return true;
	else if ((_e->isFocused() || !entityIs<Compound>(_e->focalIndex())) && entityCountOf<ConstLabel>() && entitiesOf<ConstLabel>()[0]->asKind<Label>()->keyPressed(_e))
		return true;
	return Super::keyPressed(_e);
}

}
