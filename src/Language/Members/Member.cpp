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

//#include "Class.h"
#include "DecorationContext.h"
#include "CommonGraphics.h"
#include "AccessLabel.h"
#include "Member.h"

namespace Martta
{

MARTTA_OBJECT_CPP(Member);

Access Member::access() const
{
	return isComplete() ? entityAs<AccessLabel>(0)->access() : NoAccess;
}

Kinds Member::allowedKinds(int _i) const
{
	if (_i == 0)
		return Kind::of<AccessLabel>();
	else
		return localAllowedKinds(_i - OffsetForDerivatives);
}

QString Member::code() const
{
	if (!isComplete())
		return QString();
	return entityAs<Label>(0)->code() + ": " + localCode();
}

QString Member::defineLayout(ViewKeys&/* _k*/) const
{
	return "^;m24,0,0,0;1";
}

void Member::decorate(DecorationContext const& _p) const
{
	if (!isComplete())
		return;
	_p->setPen(Qt::NoPen);
	QColor c = entityAs<AccessLabel>(0)->idColour();
	c.setAlpha(64);
	_p->setBrush(c);
	_p->drawRect(QRectF(16.f, 0.f, 4.f, _p.cap(0).height()));
	localDecorate(_p);
}

bool Member::keyPressed(EntityKeyEvent const* _e)
{
	if (entity(0)->keyPressed(_e))
		return true;
	return Super::keyPressed(_e);
}

}
