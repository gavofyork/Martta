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
#include "Const.h"
#include "MemberVariable.h"

namespace Martta
{

MARTTA_OBJECT_CPP(MemberVariable);

Kinds MemberVariable::allowedKinds(int _i) const
{
	if (_i == 2)
		return Kind::of<AccessLabel>();
	return Super::allowedKinds(_i);
}

Access MemberVariable::access() const
{
	if (!isComplete())
		return NoAccess;
	return entityAs<AccessLabel>(2)->access();
}

QString MemberVariable::code() const
{
	if (!isComplete())
		return QString();
	return entityAs<Label>(2)->code() + ": " + Super::code();
}

Type MemberVariable::type() const
{
	Type ret = Super::type();
	if (!ancestor<Class>() || !ret->isType<Reference>() || !ret->asType<Reference>()->child())
		return Type();
	ret->asType<Reference>()->child()->knitIn(new Memberify(ancestor<Class>()));
	return ret;
}

QString MemberVariable::defineLayout(ViewKeys& _v) const
{
	return "m24,0,0,0;" + Super::defineLayout(_v);
}

void MemberVariable::decorate(DecorationContext const& _p) const
{
	if (!isComplete())
		return;
	_p->setPen(Qt::NoPen);
	QColor c = entityAs<AccessLabel>(2)->idColour();
	c.setAlpha(64);
	_p->setBrush(c);
	_p->drawRect(QRectF(16.f, 0.f, 4.f, _p.cap(0).height()));
}

bool MemberVariable::keyPressed(EntityKeyEvent const* _e)
{
	if (entity(2)->keyPressed(_e))
		return true;
	return Super::keyPressed(_e);
}

bool MemberVariable::keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e)
{
	return simpleInsertionPointKeyPressHandler<MemberVariable>(_p, _e, "A");
}

}
