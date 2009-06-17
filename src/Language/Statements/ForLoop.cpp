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

#include "Primary.h"
#include "BareTyped.h"
#include "Typed.h"
#include "Compound.h"
#include "Const.h"
#include "BreakStatement.h"
#include "ForLoop.h"

namespace Martta
{

MARTTA_OBJECT_CPP(ForLoop);	
	
QString ForLoop::code() const
{
	QString ret;
	ret += "for (" + (asStatement(0) ? asStatement(0)->code() : "");
	ret += "; " + (asStatement(1) ? asStatement(1)->code() : "");
	ret += "; " + (asStatement(2) ? asStatement(2)->code() : "");
	ret += ")\n" + (asStatement(3) ? asStatement(3)->codeAsStatement() : "");
	return ret;
}

Kinds ForLoop::allowedKinds(int _index) const
{
	switch (_index)
	{
		case 0: return Kind::of<Primary>();
		case 1: return Kind::of<BareTyped>();
		case 2: return Kind::of<Typed>();
		case 3: return Kind::of<Compound>();
		default: return Super::allowedKinds(_index);
	}
}

Types ForLoop::allowedTypes(int _index) const
{
	switch (_index)
	{
		case 2: return Type(Void).topWith(Const());
		case 1: return Type(Bool).topWith(Const());
		default: return Types();
	}
}

bool ForLoop::keyPressedOnInsertionPoint(InsertionPoint const& _p, EntityKeyEvent const* _e)
{
	return simplePlaceholderKeyPressHandler<ForLoop>(_p, _e, "F");
}

bool ForLoop::keyPressed(EntityKeyEvent const* _e)
{
	if (_e->text() == ";" && _e->focalIndex() < 2)
		entity(_e->focalIndex() + 1)->setCurrent();
	else if ((_e->text() == ")" || _e->text() == "{") && _e->focalIndex() < 3 && allEntitiesOf<Compound>().size())
		allEntitiesOf<Compound>()[0]->navigateOnto(_e->codeScene());
	else if (_e->text() == "B" && _e->focus()->isPlaceholder() && _e->focus()->isAllowed<BreakStatement>())
	{
		Entity* e = new BreakStatement;
		_e->focus()->back().place(e);
		e->setCurrent();
	}
	else
		return Super::keyPressed(_e);
	return true;
}

QString ForLoop::defineLayout(ViewKeys&) const
{
	return "Hfull;ycode;^;'for (';0;'\\; ';1;'\\; ';2;')'" +
	QString(entity(3) && entity(3)->entities().size() ? ";n;i;3" : ";3");
//	QString((!entity(3) || !entity(3)->entity(0) || entity(3)->entity(0)->isPlaceholder()) ? ";' ';(;3;)" : ";n;i;(;3;)");
}

void ForLoop::decorate(DecorationContext const& /*_c*/) const
{
/*	_c->setPen(Qt::NoPen);
	_c->setBrush(QColor::fromHsv(120, 16, 236));
	QPainterPath p;
	p.addRect(_c.cap(0));
	p.addRect(_c.cap(1));
	_c->drawPath(p);*/
}

}
