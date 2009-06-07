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

#include "DecorationContext.h"
#include "CommonGraphics.h"
#include "Reference.h"
#include "Memberify.h"
#include "AccessLabel.h"
#include "ConstLabel.h"
#include "Const.h"
#include "FunctionType.h"
#include "MemberCallable.h"

namespace Martta
{

MARTTA_OBJECT_CPP(MemberCallable);
	
Access MemberCallable::access() const
{
	if (!isComplete())
		return NoAccess;
	return entitiesOf<AccessLabel>()[0]->access();
}

bool MemberCallable::isConst() const
{
	if (!isComplete())
		return NoAccess;
	return entitiesOf<ConstLabel>()[0]->isConst();
}

Kinds MemberCallable::allowedKinds(int _i) const
{
	if (_i == 3)
		return Kind::of<AccessLabel>();
	if (_i == 4)
		return Kind::of<ConstLabel>();
	return Super::allowedKinds(_i);
}

Type MemberCallable::type() const
{
	Type ret = Super::type();
	if (!ancestor<Class>() || !ret->isType<Reference>() || !ret->asType<Reference>()->child())
		return Type();
	ret->asType<Reference>()->child()->knitIn(new Memberify(ancestor<Class>(), isConst()));
	return ret;
}

QString MemberCallable::code(FunctionCodeScope _ref) const
{
	if (!isComplete())
		return QString();
	return memberCode(_ref) + (isConst() ? " const" : "");
}

QString MemberCallable::interfaceCode() const
{
	if (!isComplete())
		return QString();
	return Martta::code(access()) + ": " + Super::interfaceCode();
}

bool MemberCallable::keyPressed(EntityKeyEvent const* _e)
{
	M_ASSERT(isComplete());
	if ((_e->isFocused() || !entityIs<Compound>(_e->focalIndex())) && entitiesOf<AccessLabel>().size() && entitiesOf<AccessLabel>()[0]->asKind<Label>()->keyPressed(_e))
		return true;
	else if ((_e->isFocused() || !entityIs<Compound>(_e->focalIndex())) && entitiesOf<ConstLabel>().size() && entitiesOf<ConstLabel>()[0]->asKind<Label>()->keyPressed(_e))
		return true;
	return Super::keyPressed(_e);
}

QList<DeclarationEntity*> MemberCallable::utilised() const
{
	QList<DeclarationEntity*> l = Super::utilised();
	l.removeAll(ancestor<Class>());
	return l;
}

QString MemberCallable::defineLayout(ViewKeys& _v) const
{
	return (isConst() ? "m24,0,0,0;M-17;fs11;fb;ewhite;c#5f6f7f;'C';M9;fb0;e#0000;c#0000;fs;" : "m24,0,0,0;") + defineMemberLayout(_v);
}

void MemberCallable::decorate(DecorationContext const& _p) const
{
	_p->setPen(Qt::NoPen);
	QColor c = AccessLabel::idColour(access());
	c.setAlpha(64);
	_p->setBrush(c);
	_p->drawRect(QRectF(16.f, 0.f, 4.f, _p.cap(0).height()));
	
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
	
	Super::decorate(_p);
}

QString MemberCallable::defineMemberLayout(ViewKeys& _viewKeys) const
{
	return Super::defineLayout(_viewKeys);//.replace("#EOA", "#EOA;Mo;4;Mo");
}

}
